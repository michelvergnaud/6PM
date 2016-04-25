/*
    6PM
     Copyright (c) 2015-2016, Michel Vergnaud

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#include "MVMainWindow.h"

#include "MVVersion.h"
#include "MVSpinBoxAction.h"
#include "MVSavePresetsDialog.h"
#include "Synth/MVPlayer.h"
#include "Synth/MVControlManager.h"
#include "MVMainFrame.h"

#include <QFileDialog>
#include <QSpinBox>
#include <QMenuBar>
#include <QMenu>
#include <QSettings>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QCoreApplication>

MVMainWindow::MVMainWindow() : QMainWindow()
{
    QMenu * midiMenu =new QMenu("Midi", this);
    midiChannelAction = new MVSpinBoxAction(tr("Midi channel"));
    midiChannelAction->getSpinBox()->setMinimum(0);
    midiChannelAction->getSpinBox()->setMaximum(16);
    midiChannelAction->getSpinBox()->setSpecialValueText("Omni");
    midiMenu->addAction(midiChannelAction);

    polyphonyAction = new MVSpinBoxAction(tr("Polyphony"));
    polyphonyAction->getSpinBox()->setMinimum(1);
    polyphonyAction->getSpinBox()->setMaximum(NB_NOTES);
    midiMenu->addAction(polyphonyAction);

    pitchBendRangeAction = new MVSpinBoxAction(tr("Pitch bend range (semitones)"));
    pitchBendRangeAction->getSpinBox()->setMinimum(1);
    pitchBendRangeAction->getSpinBox()->setMaximum(24);
    pitchBendRangeAction->getSpinBox()->setValue(12);
    midiMenu->addAction(pitchBendRangeAction);

    helpMenu = new QMenu(tr("Help"), this);
    QAction * userGuideAction = new QAction(tr("User guide"),this);
    aboutAction = new QAction(tr("About"),this);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(userGuideAction);

    savePresetsDialog = NULL;

    QMenu * presetsMenu = new QMenu(tr("Presets"), this);
    QAction * modifiedPresetsAction = new QAction(tr("Modified presets"),this);
    QAction * exportAction = new QAction(tr("Export current preset"),this);
    QAction * importAction = new QAction(tr("Import preset"),this);
    QAction * resetAction = new QAction(tr("Restore factory presets"),this);
    QAction * showPresetsDirAction = new QAction(tr("Open folder"),this);

    presetsMenu->addAction(exportAction);
    presetsMenu->addAction(importAction);
    presetsMenu->addAction(modifiedPresetsAction);
    presetsMenu->addAction(resetAction);
    presetsMenu->addAction(showPresetsDirAction);

    menuBar()->addMenu(midiMenu);
    menuBar()->addMenu(presetsMenu);
    menuBar()->addSeparator();
    menuBar()->addMenu(helpMenu);

    setWindowIcon(QIcon(":/images/icon"));

    Globals::frame = new MVMainFrame(this);
    setCentralWidget(Globals::frame);

    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    Globals::frame->restoreState(settings.value("splitterSizes").toByteArray());

    connect(midiMenu,SIGNAL(aboutToShow()), this, SLOT(midiMenuAboutToShow()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    connect(userGuideAction, SIGNAL(triggered()), this, SLOT(showUserGuide()));

    connect(midiChannelAction->getSpinBox(),SIGNAL(valueChanged(int)), Globals::player,SLOT(midiChannelChanged(int)));
    connect(polyphonyAction->getSpinBox(),SIGNAL(valueChanged(int)), Globals::player,SLOT(polyphonyChanged(int)));
    connect(pitchBendRangeAction->getSpinBox(),SIGNAL(valueChanged(int)), Globals::player,SLOT(pitchBendRangeChanged(int)));

    connect(exportAction,SIGNAL(triggered()), this,SLOT(exportPreset()));
    connect(importAction,SIGNAL(triggered()), this,SLOT(importPreset()));
    connect(resetAction,SIGNAL(triggered()), this,SLOT(resetPresets()));
    connect(modifiedPresetsAction,SIGNAL(triggered()), this,SLOT(showModifiedPresetsDialog()));
    connect(showPresetsDirAction,SIGNAL(triggered()), this,SLOT(showPresetsDir()));
}

MVMainWindow::~MVMainWindow()
{
    delete midiChannelAction;
    delete polyphonyAction;
    delete pitchBendRangeAction;
    delete helpMenu;
    delete aboutAction;
    delete Globals::frame;
    delete Globals::controlmanager;
    delete Globals::player;
    delete Globals::presetManager;
}

void MVMainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("splitterSizes", Globals::frame->saveState());
    settings.setValue("maxPolyphony",Globals::player->getPolyphony());
    settings.setValue("diapason",Globals::player->getDiapason());
    settings.setValue("pitchBendRange",Globals::player->getPitchBendRange());
    settings.setValue("midiChannel",Globals::player->getMidiChannel());
    settings.setValue("userAlgos", MVPresetManager::userAlgoDescriptors);

    Globals::frame->saveSettings();

    bool bClose = true;
    if(Globals::presetManager->modifiedPresetsList().size()>0)
        bClose = showModifiedPresetsDialog();
    if(bClose)
        event->accept();
    else
        event->ignore();
}

void MVMainWindow::exportPreset()
{
    QString nameFilter = "*.mvpms";
    QString fileName = QFileDialog::getSaveFileName(this,tr("6PM Export current preset"),QDir::homePath(),nameFilter,0);
    if(fileName != "")
    {
        if(! fileName.endsWith(PRESET_FILE_EXT))
            fileName += PRESET_FILE_EXT;
        Globals::presetManager->getCurrentPreset()->saveData(fileName);
    }
}
void MVMainWindow::importPreset()
{
    QString nameFilter = "*.mvpms";
    QString fileName = QFileDialog::getOpenFileName(this,tr("6PM Import preset"),QDir::homePath(),nameFilter,0);
    if(fileName != "")
    {
        QString presetName = QFileInfo(fileName).baseName();
        QString newFileName = Globals::userPresetsDir + "/" + Globals::presetManager->getCurrentBank() + "/" + presetName + PRESET_FILE_EXT;
        if (QFile::exists(newFileName))
        {
            if(QMessageBox::question(this,tr("6PM Import preset"),tr("Overwrite preset ") + presetName + " ?", QMessageBox::Yes,QMessageBox::No) == QMessageBox::No)
                return;
            int i = 0;
            QString bakFile;
            do
                bakFile = (newFileName + "%1.bak").arg(i++);
            while(QFile::exists(bakFile));
            QFile::rename(newFileName, bakFile);
        }
        if(QFile::copy(fileName, newFileName))
        {
            Globals::presetManager->addPreset(newFileName);
            Globals::frame->addPreset(presetName);
        }
        else
        {
            QMessageBox::critical(this,tr("6PM Import preset"),tr("Impossible to import ") + presetName,QMessageBox::Close);
        }
    }
}
void MVMainWindow::resetPresets()
{
    QString bakFile;
    int i=0;
    do
    {
        bakFile = (Globals::userPresetsDir + ".bak%1").arg(i++);
    }
    while (QFile::exists(bakFile));

    if(QMessageBox::question(this,tr("6PM Import preset"),tr("Overwrite existing presets ? \n(They will be saved in %1)").arg(bakFile), QMessageBox::Yes,QMessageBox::No) == QMessageBox::No)
        return;

    QString sourceDirName = "/usr/share/6pm/Presets";
    if ( ! QFile::exists(sourceDirName))
        sourceDirName = "/usr/local/share/6pm/Presets";
    if( ! QFile::exists(sourceDirName))
    {
        QMessageBox::critical(this,tr("6PM restore presets"),tr("Impossible to find presets"), QMessageBox::Ok);
        return;
    }

    QFile::rename(Globals::userPresetsDir, bakFile);
    QString command = QString("mkdir %1").arg(Globals::userPresetsDir);
    int notok = system(command.toLatin1());
    if ( ! notok)
    {
        command = QString("cp -R %1/* %2").arg(sourceDirName).arg(Globals::userPresetsDir);
        std::cout << command.toStdString() << std::endl;

        notok = system(command.toLatin1());
    }
    if(notok)
    {
        QMessageBox::critical(this,tr("6PM restore presets"),tr("Impossible to copy presets"), QMessageBox::Ok);
        return;
    }
    Globals::presetManager->readPresets();
    Globals::frame->updateBanksAndPresets();
}
void MVMainWindow::showPresetsDir()
{
    QDesktopServices::openUrl(QUrl("file://"+Globals::userPresetsDir));
}
bool MVMainWindow::showModifiedPresetsDialog()
{
    if(savePresetsDialog != NULL) delete savePresetsDialog;
    savePresetsDialog = new MVSavePresetsDialog(this);
    return savePresetsDialog->exec();
}

void MVMainWindow::midiMenuAboutToShow()
{
    midiChannelAction->getSpinBox()->setValue(Globals::player->getMidiChannel()+1);
    polyphonyAction->getSpinBox()->setValue(Globals::player->getPolyphony());
    pitchBendRangeAction->getSpinBox()->setValue(Globals::player->getPitchBendRange());
}

void MVMainWindow::about()
{
    QString licence(" Copyright (c) 2015-2016, Michel Vergnaud \n \n\
 This program is free software; you can redistribute it and/or modify\
 it under the terms of the GNU General Public License as published by\
 the Free Software Foundation; either version 3 of the License, or\
 (at your option) any later version.\n\
\
 This program is distributed in the hope that it will be useful,\
 but WITHOUT ANY WARRANTY; without even the implied warranty of\
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\
 GNU General Public License for more details.\n\
\
 You should have received a copy of the GNU General Public License along\
 with this program; If not, see http://www.gnu.org/licenses/.\n");

    QMessageBox::information(this,tr("About 6PM"),QString("6PM Version %1\n").arg(MV6PM_VERSION)+licence);
}
void MVMainWindow::showUserGuide()
{
    QLocale locale;
    if(locale.language() == QLocale::French)
        QDesktopServices::openUrl(QUrl("/usr/share/6pm/Manuals/6PM_DocFR.pdf"));
    else
        QDesktopServices::openUrl(QUrl("/usr/share/6pm/Manuals/6PM_DocEN.pdf"));
}
