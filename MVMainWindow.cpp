/*
    6PM
     Copyright (c) 2015, Michel Vergnaud

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

#include "MVGlobals.h"
#include "MVVersion.h"
#include "MVSpinBoxAction.h"
#include "MVPlayer.h"
#include "MVControlManager.h"
#include "MVFrame.h"

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
#include <QLocale>


MVMainWindow::MVMainWindow() : QMainWindow()
{
    QCoreApplication::setOrganizationName("MVSoft");
    QCoreApplication::setApplicationName("6PM");

    // User 6pm folder
    if (! QDir(Globals::userDir).exists())
    {
        std::cout << "Creating directory  ~/.6pm" << std::endl;
        int notok = system("mkdir ~/.6pm");
        if(notok)
            std::cout << "Failed to create directory  ~/.6pm" << std::endl;
        else
        {
            std::cout << "Copying files" << std::endl;
            notok = system("cp -R /usr/share/6pm/* ~/.6pm");
        }
        if(notok)
            std::cout << "Failed to copy presets and midimaps files" << std::endl;
        else
            std::cout << "Ok" << std::endl;
    }

    QMenu * midiMenu =new QMenu("Midi", this);
    midiChannelAction = new MVSpinBoxAction("Midi channel");
    midiChannelAction->getSpinBox()->setMinimum(0);
    midiChannelAction->getSpinBox()->setMaximum(16);
    midiChannelAction->getSpinBox()->setSpecialValueText("Omni");
    midiMenu->addAction(midiChannelAction);

    polyphonyAction = new MVSpinBoxAction("Polyphony");
    polyphonyAction->getSpinBox()->setMinimum(1);
    polyphonyAction->getSpinBox()->setMaximum(NB_NOTES);
    midiMenu->addAction(polyphonyAction);

    pitchBendRangeAction = new MVSpinBoxAction("Pitch bend range (semitones)");
    pitchBendRangeAction->getSpinBox()->setMinimum(1);
    pitchBendRangeAction->getSpinBox()->setMaximum(24);
    pitchBendRangeAction->getSpinBox()->setValue(12);
    midiMenu->addAction(pitchBendRangeAction);

    helpMenu = new QMenu(tr("Help"), this);
    QAction * userGuideAction = new QAction("User guide",this);
    aboutAction = new QAction("About",this);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(userGuideAction);

    QMenu * presetsMenu = new QMenu(tr("Presets"), this);
    QAction * exportAction = new QAction("Export preset",this);
    QAction * importAction = new QAction("Imort preset",this);
    QAction * midiExportAction = new QAction("Export midi map",this);
    QAction * midiImportAction = new QAction("Import midi map",this);

    presetsMenu->addAction(exportAction);
    presetsMenu->addAction(importAction);
    presetsMenu->addAction(midiExportAction);
    presetsMenu->addAction(midiImportAction);

    menuBar()->addMenu(midiMenu);
//    menuBar()->addMenu(presetsMenu);
    menuBar()->addSeparator();
    menuBar()->addMenu(helpMenu);

    setWindowIcon(QIcon(":/images/icon"));

    Globals::player = new MVPlayer();
    Globals::controlmanager = new MVControlManager();
    Globals::frame = new MVFrame(this);
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
}

MVMainWindow::~MVMainWindow()
{
    delete Globals::frame;
    delete Globals::controlmanager;
    delete Globals::player;
    delete midiChannelAction;
    delete polyphonyAction;
    delete pitchBendRangeAction;
    delete helpMenu;
    delete aboutAction;
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

    QMainWindow::closeEvent(event);
}

void MVMainWindow::midiMenuAboutToShow()
{
    midiChannelAction->getSpinBox()->setValue(Globals::player->getMidiChannel()+1);
    polyphonyAction->getSpinBox()->setValue(Globals::player->getPolyphony());
    pitchBendRangeAction->getSpinBox()->setValue(Globals::player->getPitchBendRange());
}

void MVMainWindow::about()
{
    QString licence(" Copyright (c) 2015, Michel Vergnaud \n \n\
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

    QMessageBox::information(this,"About 6PM",QString("6PM Version %1\n").arg(MV6PM_VERSION)+licence);
}
void MVMainWindow::showUserGuide()
{
    QLocale locale;
    if(locale.language() == QLocale::French)
        QDesktopServices::openUrl(QUrl("/usr/share/6pm/Manuals/6PM_DocFR.pdf"));
    else
        QDesktopServices::openUrl(QUrl("/usr/share/6pm/Manuals/6PM_DocEN.pdf"));
}
