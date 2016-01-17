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

#include "MVFrame.h"
#include "MVNote.h"
#include "MVControlManager.h"
#include "MVPlayer.h"
#include "MVOscillator_ui.h"
#include "MVEnvelope_ui.h"
#include "MVDial.h"
#include "MVFreqEnvelope.h"
#include "MVEnvelopeDrawer.h"
#include "MVPushButton.h"
#include "MVMeter.h"
#include "MVAlgoDrawer.h"
#include "MVBackground.h"

#include <iostream>
#include <cstdlib>

#include <QFileDialog>
#include <QLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QGradient>
#include <QSettings>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QMessageBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QSplitter>


MVFrame::MVFrame(QWidget *parent) : QSplitter(parent)
{
    Globals::frame = this;
    QSettings settings;
    QWidget * rightPanel = new QWidget();
    setStyleSheet("QLabel{color: yellow}");
    MVBackground::setColors(Qt::darkGray, QColor::fromRgb(144, 128, 128), QColor::fromRgb(32,32,32));

    QGridLayout * mainLayout = new QGridLayout();
    mainLayout->setSpacing(6);
    mainLayout->setContentsMargins(2,2,2,2);
    rightPanel->setLayout(mainLayout);

    cbMidiMap = new QComboBox(this);
    updateCbMidiMap();
    if(midiMapList.count()>0)
        currentBank = midiMapList.at(0).absoluteFilePath();

    QWidget * midiMapButtonsWidget = new QWidget(this);
    QGridLayout * midiMapButtonsBox = new QGridLayout;
    midiMapButtonsWidget->setLayout(midiMapButtonsBox);

    butSaveMidiMap = new MVPushButton("Save", this);
    butSaveMidiMapAs = new MVPushButton("Save as", this);
    butRenameMidiMap = new MVPushButton("Rename", this);
    butDeleteMidiMap = new MVPushButton("Delete", this);
    midiMapButtonsBox->addWidget(butSaveMidiMap,0,0);
    midiMapButtonsBox->addWidget(butSaveMidiMapAs,0,1);
    midiMapButtonsBox->addWidget(butRenameMidiMap,1,0);
    midiMapButtonsBox->addWidget(butDeleteMidiMap,1,1);

    MVBackground * algoButtonsWidget = new MVBackground();
    QGridLayout * algoButtonsLayout = new QGridLayout;
    algoButtonsWidget->setLayout(algoButtonsLayout);
    radioPreDef = new QRadioButton("Pre-defined",algoButtonsWidget);
    radioUser = new QRadioButton("User", algoButtonsWidget);
    radioPreDef->setChecked(settings.value("PreDefinedAlgo",true).toBool());
    radioUser->setChecked(! radioPreDef->isChecked());
    butEditAlgo = new MVPushButton("Edit", algoButtonsWidget);
    butEditAlgo->setEnabled(radioUser->isChecked());
    butCancelEditAlgo = new MVPushButton("Cancel", algoButtonsWidget);
    butCancelEditAlgo->setEnabled(radioUser->isChecked());
    butSaveAlgo = new MVPushButton("Save", algoButtonsWidget);
    butSaveAlgo->setEnabled(radioUser->isChecked());
    butAddAlgo = new MVPushButton("Add", algoButtonsWidget);
    butAddAlgo->setEnabled(radioUser->isChecked());

    algoButtonsLayout->addWidget(radioPreDef,0,0);
    algoButtonsLayout->addWidget(radioUser,0,1);
    algoButtonsLayout->addWidget(butEditAlgo,1,0);
    algoButtonsLayout->addWidget(butCancelEditAlgo,1,1);
    algoButtonsLayout->addWidget(butSaveAlgo,2,0);
    algoButtonsLayout->addWidget(butAddAlgo,2,1);

    cbAlgo = new QComboBox(this);
    for(int i=0;i<NB_ALGO;i++)
        cbAlgo->addItem("Algorithm "+QString::number(i+1));

    algoDrawer = new MVAlgoDrawer();
    algoDrawer->setMinimumHeight(100);
    cbBank = new QComboBox(this);
    QFont font = cbBank->font();
    font.setPointSize(10);
    cbBank->setFont(font);
    updateCbBank();
    butNewBank = new MVPushButton("New bank", this);
    butRenameBank = new MVPushButton("Rename", this);

    cbPreset = new QComboBox(this);
    font = cbPreset->font();
    font.setPointSize(15);
    cbPreset->setFont(font);
    updateCbPresets();

    QWidget * fileButtonsWidget = new QWidget(this);
    QGridLayout * fileButtonsBox = new QGridLayout;
    fileButtonsWidget->setLayout(fileButtonsBox);

    MVPushButton::setColors(Qt::black, Qt::darkGray);

    butSavePreset = new MVPushButton("Save", this);
    butSavePresetAs = new MVPushButton("Save as",this);
    butRenamePreset = new MVPushButton("Rename",this);
    butDeletePreset = new MVPushButton("Delete",this);
    fileButtonsBox->addWidget(butSavePreset,0,0);
    fileButtonsBox->addWidget(butSavePresetAs,0,1);
    fileButtonsBox->addWidget(butRenamePreset,1,0);
    fileButtonsBox->addWidget(butDeletePreset,1,1);

    MVEnvelope_UI::timeValues = new float[MVEnvelope_UI::nbTimeValues];

    for(int i=0;i<10;i++)
        MVEnvelope_UI::timeValues[i] = (i+1);
    for(int i=10;i<28;i++)
        MVEnvelope_UI::timeValues[i] = (i-10+1)*5 + 10;
    for(int i=28;i<46;i++)
        MVEnvelope_UI::timeValues[i] = (i-28+1)*50 + 100;
    for(int i=46;i<=63;i++)
        MVEnvelope_UI::timeValues[i] = (i-46+1)*500 + 1000;

    oscUI = new MVOscillator_UI*[NB_OSC];
    envAmpUI = new MVEnvelope_UI*[NB_OSC];
    envFreqUI = new MVEnvelope_UI*[NB_OSC];

    for(int i=0;i<NB_OSC;i++)
    {
        QHBoxLayout * oscLayout = new QHBoxLayout();
        oscLayout->setSpacing(2);
        oscLayout->setContentsMargins(0,0,0,0);
        oscBg[i] = new MVBackground();
        oscBg[i]->setLayout(oscLayout);

        oscUI[i] = new MVOscillator_UI(MVNote::oscData[i], i);

        envAmpUI[i] = new MVEnvelope_UI(MVAmpEnvelope::envAmpData[i], i, MVEnvelope_UI::AMP);
        envFreqUI[i] = new MVEnvelope_UI(MVFreqEnvelope::envFreqData[i],i,MVEnvelope_UI::FREQ);

        oscLayout->addWidget(oscUI[i]);

        QWidget * envWidget = new QWidget(oscBg[i]);
        QVBoxLayout * envLayout = new QVBoxLayout();
        envLayout->setSpacing(0);
        envLayout->setContentsMargins(0,0,0,0);

        QLabel * label = new QLabel(QString::number(i+1));
        label->setAlignment(Qt::AlignCenter);
        envLayout->addWidget(label);

        envWidget->setLayout(envLayout);
        envLayout->addWidget(envAmpUI[i]);
        envLayout->addWidget(envFreqUI[i]);
        envLayout->setStretchFactor(envAmpUI[i],1);
        envLayout->setStretchFactor(envFreqUI[i],1);

        oscLayout->addWidget(envWidget);
        oscLayout->setStretchFactor(oscUI[i],2);
        oscLayout->setStretchFactor(envWidget,5);
        mainLayout->addWidget(oscBg[i],i/3,i%3);
    }

    MVBackground * leftPanel = new MVBackground();
    QVBoxLayout * leftPanelLayout = new QVBoxLayout();
    leftPanel->setLayout(leftPanelLayout);

    QLabel * labelMidiMap  = new QLabel("Midi map");
    leftPanelLayout->addWidget(labelMidiMap,1);
    leftPanelLayout->addWidget(cbMidiMap,1);
    leftPanelLayout->addWidget(midiMapButtonsWidget,1);

    QLabel * labelAlgo  = new QLabel("Algorithm");
    leftPanelLayout->addWidget(labelAlgo,1);
    leftPanelLayout->addWidget(cbAlgo,1);
    leftPanelLayout->addWidget(algoDrawer,10);
    leftPanelLayout->addWidget(algoButtonsWidget,1);

    QLabel * labelBank  = new QLabel("Bank");
    leftPanelLayout->addWidget(labelBank,1);
    leftPanelLayout->addWidget(cbBank,1);

    QWidget * bankButtonsWidget = new QWidget(this);
    QHBoxLayout * bankButtonsLayout = new QHBoxLayout();
    bankButtonsWidget->setLayout(bankButtonsLayout);
    bankButtonsLayout->addWidget(butNewBank);
    bankButtonsLayout->addWidget(butRenameBank);
    leftPanelLayout->addWidget(bankButtonsWidget,1);

    QLabel * labelPreset  = new QLabel("Preset");
    leftPanelLayout->addWidget(labelPreset,1);
    leftPanelLayout->addWidget(cbPreset,1);
    leftPanelLayout->addWidget(fileButtonsWidget,1);

    MVBackground* volWidget = new MVBackground();

    meterL = new MVMeter(Globals::player->getBufferL(),Globals::player->getBufferSize());
    meterL->setMinimumWidth(10);
    meterR = new MVMeter(Globals::player->getBufferR(),Globals::player->getBufferSize());
    meterR->setMinimumWidth(10);

    MVBackground * buttsWidget = new MVBackground();
    QVBoxLayout * buttsLayout = new QVBoxLayout(buttsWidget);
    buttsLayout->setContentsMargins(1,2,1,2);
    buttsLayout->setSpacing(0);
    buttsWidget->setLayout(buttsLayout);
    dialDiapason = new MVDial(buttsWidget,4400);

    dialDiapason->setMinimum(4200);
    dialDiapason->setMaximum(4600);
    dialDiapason->setValue(4400);
    dialDiapason->setLabel("440");
    dialDiapason->setLabelColor(Qt::green);
    dialDiapason->setMinimumSize(40,40);

    butTest = new MVPushButton("Test",this);

    butPanic = new QPushButton(QIcon(":/images/stop"),"");
    butPanic->setStyleSheet("background-color: rgb(255, 0, 0);");

    buttsLayout->addWidget(new QLabel("Diapason"),1);
    buttsLayout->addWidget(dialDiapason,2);
    buttsLayout->addWidget(butTest);

    buttsLayout->addWidget(new QWidget, 10);
    buttsLayout->addWidget(new QLabel("Panic"),1);
    buttsLayout->addWidget(butPanic,3);

    QHBoxLayout * volLayout = new QHBoxLayout();
    volWidget->setLayout(volLayout);
    volSlider = new MVSlider();
    volSlider->setValue(settings.value("mainVolume", 50 ).toInt());

    volLayout->setContentsMargins(2,2,2,2);
    volLayout->addWidget(buttsWidget,3);
    volLayout->addWidget(meterL,1);
    volLayout->addWidget(meterR,1);
    volLayout->addWidget(volSlider,4);
    volLayout->addWidget(new QWidget(),10);
    leftPanelLayout->addWidget(volWidget,5);

    addWidget(leftPanel);
    addWidget(rightPanel);

    setChildrenCollapsible(false);

    updateUIs();

    QString lastMidiMap = settings.value("lastMidiMap").toString();
    loadMidiMap(lastMidiMap);

    int lastBankNum = settings.value("lastBank",0).toInt();

    if(lastBankNum < bankList.count())
    {
        currentBank = bankList.at(lastBankNum).absoluteFilePath();
        cbBank->setCurrentIndex(lastBankNum);
        updateCbPresets();
    }
    MVNote::userAlgoDescriptor = settings.value("userAlgos").toStringList();
    QString lastPreset = settings.value("lastPreset").toString();
    loadPreset(lastPreset);

    connect(cbMidiMap,SIGNAL(currentIndexChanged(int)),this,SLOT(midiMapChanged(int)));
    connect(cbAlgo,SIGNAL(currentIndexChanged(int)),this,SLOT(algoChanged(int)));
    connect(butEditAlgo,SIGNAL(clicked()),this,SLOT(startEditAlgo()));
    connect(butCancelEditAlgo,SIGNAL(clicked()),this,SLOT(cancelEditAlgo()));
    connect(butSaveAlgo,SIGNAL(clicked()),this,SLOT(saveAlgo()));
    connect(butAddAlgo,SIGNAL(clicked()),this,SLOT(addAlgo()));
    connect(radioPreDef,SIGNAL(toggled(bool)),this,SLOT(algoTypeChanged()));

    connect(cbPreset,SIGNAL(currentIndexChanged(int)),this,SLOT(presetChanged(int)));
    connect(cbBank,SIGNAL(currentIndexChanged(int)),this,SLOT(bankChanged(int)));
    connect(volSlider,SIGNAL(valueChanged(int)),this,SLOT(mainVolChanged(int)));
    connect(butSaveMidiMap,SIGNAL(clicked()),this,SLOT(onButtSaveMidiMap()));
    connect(butSaveMidiMapAs,SIGNAL(clicked()),this,SLOT(onButtSaveMidiMapAs()));
    connect(butRenameMidiMap,SIGNAL(clicked()),this,SLOT(onButtRenameMidiMap()));
    connect(butDeleteMidiMap,SIGNAL(clicked()),this,SLOT(onButtDeleteMidiMap()));

    connect(butSavePreset,SIGNAL(clicked()),this,SLOT(onButtSavePreset()));
    connect(butSavePresetAs,SIGNAL(clicked()),this,SLOT(onButtSavePresetAs()));
    connect(butRenamePreset,SIGNAL(clicked()),this,SLOT(onButtRenamePreset()));
    connect(butDeletePreset,SIGNAL(clicked()),this,SLOT(onButtDeletePreset()));

    connect(butNewBank,SIGNAL(clicked()),this,SLOT(onButtNewBank()));
    connect(butRenameBank,SIGNAL(clicked()),this,SLOT(onButtRenameBank()));

    connect(butPanic,SIGNAL(pressed()),this,SLOT(onButtPanic()));
    connect(dialDiapason,SIGNAL(valueChanged(int)),this,SLOT(diapasonChanged(int)));
    connect(butTest,SIGNAL(pressed()),this,SLOT(onPlayTest()));
    connect(butTest,SIGNAL(released()),this,SLOT(onStopTest()));
}

MVFrame::~MVFrame()
{
    QSettings settings;
    settings.setValue("mainVolume", volSlider->value());
    settings.setValue("userAlgos", MVNote::userAlgoDescriptor);

    delete[] oscUI;
    delete[] envAmpUI;
    delete[] envFreqUI;
}

void MVFrame::startEditAlgo()
{
    algoDrawer->startEdit();
    butCancelEditAlgo->setEnabled(true);
    butSaveAlgo->setEnabled(true);
    butAddAlgo->setEnabled(true);
}
void MVFrame::cancelEditAlgo()
{
    algoDrawer->endEdit(true);
    butCancelEditAlgo->setEnabled(false);
    butSaveAlgo->setEnabled(false);
    butAddAlgo->setEnabled(false);
}

void MVFrame::algoChanged(int a)
{
    algoDrawer->endEdit(false);
    MVNote::algoType = radioPreDef->isChecked() ? MVNote::PREDEF : MVNote::USER;
    if(MVNote::algoType == MVNote::PREDEF)
    {
        MVNote::preDefAlgoIndex = a;
        MVNote::createAlgo();
    }
    else
    {
        MVNote::userAlgoIndex = a;
        MVNote::createAlgo(MVNote::userAlgoDescriptor.at(a));
    }
    algoDrawer->update();
    updateUIs();
}
void MVFrame::addAlgo()
{
    MVNote::userAlgoDescriptor.append(algoDrawer->getAlgoDescriptor());
    MVNote::createAlgo(algoDrawer->getAlgoDescriptor());
    algoDrawer->endEdit(false);
    cbAlgo->addItem(QString("User algo "+ QString::number(MVNote::userAlgoDescriptor.count())));
    cbAlgo->setCurrentIndex(MVNote::userAlgoDescriptor.count()-1);
}
void MVFrame::saveAlgo()
{
    int i = cbAlgo->currentIndex();
    if(i > -1)
        MVNote::userAlgoDescriptor[i] = algoDrawer->getAlgoDescriptor();
    else
        addAlgo();
    algoDrawer->endEdit(false);
}

void MVFrame::updateCbAlgo()
{
    cbAlgo->disconnect(this);
    cbAlgo->clear();
    if(MVNote::algoType == MVNote::PREDEF)
    {
        algoDrawer->endEdit(false);
        for(int i=0;i<NB_ALGO;i++)
            cbAlgo->addItem("Algorithm "+QString::number(i+1));
        cbAlgo->setCurrentIndex(MVNote::preDefAlgoIndex);
    }
    else
    {
        for(int i=0;i<MVNote::userAlgoDescriptor.count();i++)
            cbAlgo->addItem("User algo "+QString::number(i+1));
        cbAlgo->setCurrentIndex(MVNote::userAlgoIndex);
    }
    connect(cbAlgo,SIGNAL(currentIndexChanged(int)),this,SLOT(algoChanged(int)));
}

void MVFrame::algoTypeChanged()
{
    bool b = radioPreDef->isChecked();
    MVNote::algoType = b ? MVNote::PREDEF : MVNote::USER;
    butEditAlgo->setEnabled( ! b);
    if( b )
    {
        butCancelEditAlgo->setEnabled(false);
        butSaveAlgo->setEnabled( false);
        butAddAlgo->setEnabled( false);
    }

    if ( b )
        MVNote::createAlgo();
    else
        MVNote::createAlgo(MVNote::userAlgoIndex);

    updateCbAlgo();
    algoDrawer->update();
}

void MVFrame::mainVolChanged(int v)
{
    MVPlayer::mainVolume = v/100.0;
}

void MVFrame::diapasonChanged(int n)
{
    float newDiapason = (float)n / 10.0;
    Globals::player->setDiapason(newDiapason);
    dialDiapason->setLabel(QString::number(newDiapason,'f',2));
}

void MVFrame::bankChanged(int n)
{
    if(n >= 0 && n < cbBank->count())
    {
        currentBank = Globals::userPresetsDir + "/" + bankList.at(n).baseName();
        cbPreset->disconnect();
        updateCbPresets();
        cbPreset->setCurrentIndex(-1);
        connect(cbPreset,SIGNAL(currentIndexChanged(int)),this,SLOT(presetChanged(int)));
        QSettings settings;
        settings.setValue("lastBank", n);
    }
}

void MVFrame::midiMapChanged(int n)
{
    if(n >= 0 && n < midiMapList.count())
    {
        QString fileName = midiMapList.at(n).absoluteFilePath();
        loadMidiMap(fileName);
        QSettings settings;
        settings.setValue("lastMidiMap",fileName);
    }
    cbMidiMap->setEditable(false);
}

void MVFrame::presetChanged(int n)
{
    if(n >= 0 && n < presetList.count())
    {
        QString fileName = presetList.at(n).absoluteFilePath();
        loadPreset(fileName);
        QSettings settings;
        settings.setValue("lastPreset",fileName);
    }
    cbPreset->setEditable(false);
}

void MVFrame::updateCbBank()
{
    cbBank->clear();
    QDir rootDir(Globals::userPresetsDir);
    bankList = rootDir.entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot );
    foreach(QFileInfo f, bankList)
    {
        QString item = f.baseName();
        cbBank->addItem(item);
    }
}

void MVFrame::updateCbPresets()
{
    cbPreset->clear();

    QDir dir(Globals::userPresetsDir + "/" + cbBank->currentText());
    QStringList nameFilter;
    nameFilter << "*.mvpms";
    presetList = dir.entryInfoList( nameFilter, QDir::Files );
    foreach(QFileInfo f, presetList)
    {
        QString item = f.completeBaseName();
        cbPreset->addItem(item);
    }
}

void MVFrame::updateCbMidiMap()
{
    cbMidiMap->clear();
    QDir dir(Globals::userMidiMapsDir);
    QStringList nameFilter;
    nameFilter << "*.mvmm";
    midiMapList = dir.entryInfoList( nameFilter, QDir::Files );

    foreach(QFileInfo f, midiMapList)
    {
        QString item = f.baseName();
        cbMidiMap->addItem(item);
    }
}

void MVFrame::onButtSaveMidiMap()
{
    QString fileName = Globals::userMidiMapsDir + "/" + cbMidiMap->currentText() + ".mvmm";
    QString fileNameBak = fileName+ ".bak";
    QFile::copy(fileName, fileNameBak);
    saveMidiMap(fileName);
}
void MVFrame::onButtSaveMidiMapAs()
{
    editSender = butSaveMidiMapAs;
    cbMidiMap->setEditable(true);
    connect(cbMidiMap->lineEdit(),SIGNAL(returnPressed()),this,SLOT(midiMapReturnPressed()));
    connect(cbMidiMap->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingMidiMapFinished()));
    cbMidiMap->setFocus();
}
void MVFrame::onButtRenameMidiMap()
{
    editSender = butRenameMidiMap;
    oldFileName = Globals::userMidiMapsDir + "/" + cbMidiMap->currentText() + ".mvmm";
    cbMidiMap->setEditable(true);
    connect(cbMidiMap->lineEdit(),SIGNAL(returnPressed()),this,SLOT(midiMapReturnPressed()));
    connect(cbMidiMap->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingMidiMapFinished()));
    cbMidiMap->setFocus();
}
void MVFrame::onButtDeleteMidiMap()
{
    QString fileName = Globals::userMidiMapsDir + "/" + cbMidiMap->currentText() + ".mvmm";
    if (QMessageBox::question(this, "Delete midi map","Are you sure ?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
    {
        QFile::copy(fileName, fileName + ".bak");
        QFile::remove(fileName);
        updateCbMidiMap();
    }
}
void MVFrame::midiMapReturnPressed()
{
    cbMidiMap->lineEdit()->disconnect();
    QString newName = cbMidiMap->currentText();
    QString newFileName = Globals::userMidiMapsDir + "/" + newName + ".mvmm";
    if(editSender == butRenameMidiMap)
    {
        bool ok = QFile::rename(oldFileName, newFileName);
        updateCbMidiMap();
        if( ok)
            cbMidiMap->setCurrentText(newName);
        else
        {
            QMessageBox::warning(NULL, "Rename midi map","Impossible to rename file");
            cbMidiMap->setCurrentText(QFileInfo(oldFileName).baseName());
        }
    }
    else
    {
        bool saveIt = true;
        if ( QFile::exists(newFileName))
        {
            saveIt = QMessageBox::question(this, "Save midi map","Replace file ?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes;
        }
        if(saveIt)
        {
            saveMidiMap(newFileName);
            updateCbMidiMap();
            cbMidiMap->setCurrentText(newName);
        }
    }
}

void MVFrame::editingMidiMapFinished()
{
    cbMidiMap->lineEdit()->disconnect();
    cbMidiMap->setEditable(false);
}

void MVFrame::onButtSavePreset()
{
    if(algoDrawer->isEditing())
        saveAlgo();
    QString fileName = currentBank + "/" + cbPreset->currentText() + ".mvpms";
    QString fileNameBak = fileName+ ".bak";
    QFile::copy(fileName, fileNameBak);
    savePreset(fileName);
}

void MVFrame::onButtSavePresetAs()
{
    editSender = butSavePresetAs;
    cbPreset->setEditable(true);
    connect(cbPreset->lineEdit(),SIGNAL(returnPressed()),this,SLOT(presetReturnPressed()));
    connect(cbPreset->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingPresetFinished()));
    cbPreset->setFocus();
}

void MVFrame::onButtRenamePreset()
{
    editSender = butRenamePreset;
    oldFileName = currentBank + "/" + cbPreset->currentText() + ".mvpms";
    cbPreset->setEditable(true);
    connect(cbPreset->lineEdit(),SIGNAL(returnPressed()),this,SLOT(presetReturnPressed()));
    connect(cbPreset->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingPresetFinished()));
    cbPreset->setFocus();
}
void MVFrame::onButtDeletePreset()
{
    QString fileName = currentBank + "/" + cbPreset->currentText() + ".mvpms";
    if (QMessageBox::question(this, "Delete preset","Are you sure ?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
    {
        QFile::copy(fileName, fileName + ".bak");
        QFile::remove(fileName);
        updateCbPresets();
    }
}

void MVFrame::presetReturnPressed()
{
    cbPreset->lineEdit()->disconnect();
    QString newName = cbPreset->currentText();
    QString newFileName = currentBank + "/" + newName + ".mvpms";

    if(editSender == butRenamePreset)
    {
        bool ok = QFile::rename(oldFileName, newFileName);
        updateCbPresets();
        if( ok)
            cbPreset->setCurrentIndex(presetList.indexOf(newFileName));
        else
        {
            QMessageBox::warning(NULL, "Rename preset","Impossible to rename file");
            cbPreset->setCurrentIndex(presetList.indexOf(oldFileName));
        }
    }
    else
    {
        bool saveIt = true;
        if ( QFile::exists(newFileName))
            saveIt = QMessageBox::question(this, "Save preset","Replace file ?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes;
        if(saveIt)
        {
            savePreset(newFileName);
            updateCbPresets();
            cbPreset->setCurrentIndex(presetList.indexOf(newFileName));
        }
    }
}

void MVFrame::editingPresetFinished()
{
    cbPreset->lineEdit()->disconnect();
    cbPreset->setEditable(false);
}

void MVFrame::onButtNewBank()
{
    editSender = butNewBank;
    cbBank->disconnect();
    cbBank->setEditable(true);
    connect(cbBank->lineEdit(),SIGNAL(returnPressed()),this,SLOT(bankReturnPressed()));
    connect(cbBank->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingNewBankFinished()));
    cbBank->setFocus();
}
void MVFrame::onButtRenameBank()
{
    editSender = butRenameBank;
    oldFileName = Globals::userPresetsDir + "/" + cbBank->currentText();
    cbBank->disconnect();
    cbBank->setEditable(true);
    connect(cbBank->lineEdit(),SIGNAL(returnPressed()),this,SLOT(bankReturnPressed()));
    connect(cbBank->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingNewBankFinished()));
    cbBank->setFocus();
}
void MVFrame::bankReturnPressed()
{
    cbBank->lineEdit()->disconnect();
    cbBank->disconnect();
    cbBank->setEditable(false);
    QString newName = cbBank->currentText();
    if( ! newName.isEmpty())
    {
        QString bankDir(Globals::userPresetsDir);
        if(editSender == butNewBank)
        {
            QDir(bankDir).mkdir(newName);
        }
        else
        //(editSender == butRenameBank)
        {
            newName = bankDir + newName;
            bool ok = QDir(bankDir).rename(oldFileName, newName);
            if( ok)
            {
                updateCbBank();
                cbBank->setCurrentText(newName);
            }
            else
            {
                QMessageBox::warning(NULL, "Rename bank","Impossible to rename directory");
                cbBank->setCurrentText(oldFileName);
            }
        }
        updateCbBank();
        updateCbPresets();
        cbBank->setCurrentText(newName);
        connect(cbBank,SIGNAL(currentIndexChanged(int)),this,SLOT(bankChanged(int)));

    }
}
void MVFrame::editingNewBankFinished()
{
    cbBank->lineEdit()->disconnect();
    cbBank->setEditable(false);
}

// Called from Control Change
void MVFrame::loadPreset(int n)
{
    if(n>=0 && n < presetList.count())
        cbPreset->setCurrentIndex(n);
}
// Called from Control Change
void MVFrame::bankSelect(int n)
{
    if(n>=0 && n < cbBank->count())
        cbBank->setCurrentIndex(n);
}

void MVFrame::loadMidiMap(QString fileName)
{
    QSettings settings(fileName, QSettings::NativeFormat);
    Globals::controlmanager->registerAssign(settings.value(QString("mainVolume"), 7).toInt(), volSlider);

    for(int i=0;i<NB_OSC;i++)
    {
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_pan").arg(i),0).toInt(), oscUI[i]->sliderPan);
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_amp").arg(i),0).toInt(), oscUI[i]->dialAmp);
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_freqratio").arg(i),0).toInt(), oscUI[i]->dialFreqRatio);
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_detune").arg(i),0).toInt(), oscUI[i]->dialDetune);
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_random").arg(i),0).toInt(), oscUI[i]->dialRandom);
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_iMod").arg(i),0).toInt(), oscUI[i]->dialIMod);
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_tremAmp").arg(i),0).toInt(), oscUI[i]->dialTremoloAmp);
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_tremFreq").arg(i),0).toInt(), oscUI[i]->dialTremoloFreq);
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_tremShape").arg(i),0).toInt(), oscUI[i]->dialTremoloShape);
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_vibAmp").arg(i),0).toInt(), oscUI[i]->dialVibratoAmp);
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_vibFreq").arg(i),0).toInt(), oscUI[i]->dialVibratoFreq);
        Globals::controlmanager->registerAssign(settings.value(QString("osc%1midiCC_vibShape").arg(i),0).toInt(), oscUI[i]->dialVibratoShape);

        for (int j=0;j<NB_ENV_PTS;j++)
        {
            if(j>0)
            {
                Globals::controlmanager->registerAssign(settings.value(QString("envAmp%1%2midiCC_frame").arg(i).arg(j),0).toInt(), envAmpUI[i]->dialsTime[j]);
                Globals::controlmanager->registerAssign(settings.value(QString("envFreq%1%2midiCC_frame").arg(i).arg(j),0).toInt(), envFreqUI[i]->dialsTime[j]);

            }
            if(j<NB_ENV_PTS-1)
            {
                Globals::controlmanager->registerAssign(settings.value(QString("envAmp%1%2midiCC_value").arg(i).arg(j),0).toInt(), envAmpUI[i]->dialsValue[j]);
                Globals::controlmanager->registerAssign(settings.value(QString("envFreq%1%2midiCC_value").arg(i).arg(j),0).toInt(), envFreqUI[i]->dialsValue[j]);
            }
        }
        updateUIs();
    }

    int midiMapNum = midiMapList.indexOf(fileName);
    if(midiMapNum <0) midiMapNum = 0;
    cbMidiMap->setCurrentIndex(midiMapNum);
}

void MVFrame::saveMidiMap(QString fileName)
{
    if (! fileName.endsWith((".mvmm")))
        fileName += ".mvmm";
    QSettings settings(fileName,QSettings::NativeFormat);
    settings.setValue(QString("mainVolume"), volSlider->getAssignNumber());

    for(int i=0;i<NB_OSC;i++)
    {
        settings.setValue(QString("osc%1midiCC_pan").arg(i), oscUI[i]->sliderPan->getAssignNumber());
        settings.setValue(QString("osc%1midiCC_amp").arg(i), oscUI[i]->dialAmp->getAssignNumber());
        settings.setValue(QString("osc%1midiCC_freqratio").arg(i), oscUI[i]->dialFreqRatio->getAssignNumber());
        settings.setValue(QString("osc%1midiCC_detune").arg(i), oscUI[i]->dialDetune->getAssignNumber());
        settings.setValue(QString("osc%1midiCC_random").arg(i), oscUI[i]->dialRandom->getAssignNumber());
        settings.setValue(QString("osc%1midiCC_iMod").arg(i), oscUI[i]->dialIMod->getAssignNumber());
        settings.setValue(QString("osc%1midiCC_tremAmp").arg(i), oscUI[i]->dialTremoloAmp->getAssignNumber());
        settings.setValue(QString("osc%1midiCC_tremFreq").arg(i), oscUI[i]->dialTremoloFreq->getAssignNumber());
        settings.setValue(QString("osc%1midiCC_tremShape").arg(i), oscUI[i]->dialTremoloShape->getAssignNumber());
        settings.setValue(QString("osc%1midiCC_vibAmp").arg(i), oscUI[i]->dialVibratoAmp->getAssignNumber());
        settings.setValue(QString("osc%1midiCC_vibFreq").arg(i), oscUI[i]->dialVibratoFreq->getAssignNumber());
        settings.setValue(QString("osc%1midiCC_vibShape").arg(i), oscUI[i]->dialVibratoShape->getAssignNumber());

        for (int j=0;j<NB_ENV_PTS;j++)
        {
            if(j>0)
            {
                settings.setValue(QString("envAmp%1%2midiCC_frame").arg(i).arg(j), envAmpUI[i]->dialsTime[j]->getAssignNumber());
                settings.setValue(QString("envAmp%1%2midiCC_frame").arg(i).arg(j), envFreqUI[i]->dialsTime[j]->getAssignNumber());
            }
            if(j<NB_ENV_PTS-1)
            {
                settings.setValue(QString("envAmp%1%2midiCC_value").arg(i), envAmpUI[i]->dialsValue[j]->getAssignNumber());
                settings.setValue(QString("envAmp%1%2midiCC_value").arg(i), envFreqUI[i]->dialsValue[j]->getAssignNumber());
            }
        }
    }
}

void MVFrame::loadPreset(QString fileName)
{
    QSettings settings(fileName, QSettings::NativeFormat);
    MVNote::algoType = settings.value("PreDefinedAlgo",true).toBool() ? MVNote::PREDEF : MVNote::USER;
    if(MVNote::algoType == MVNote::PREDEF)
    {
        MVNote:: preDefAlgoIndex = settings.value(QString("numAlgo"),0).toInt();
    }
    else
    {
        QString descriptor = settings.value(QString("algoDescriptor"),"1").toString();
        int numAlgo = MVNote::getUserAlgoIndex(descriptor);
        if(numAlgo > -1)
            MVNote::userAlgoIndex = numAlgo;
        else
            MVNote::userAlgoIndex = MVNote::addUserAlgo(descriptor);
    }
    radioPreDef->setChecked(MVNote::algoType == MVNote::PREDEF);
    radioUser->setChecked(MVNote::algoType == MVNote::USER);
    updateCbAlgo();

    int presetNum = presetList.indexOf(fileName);
    if(presetNum <0) presetNum = 0;
    cbPreset->setCurrentIndex(presetNum);
    cbAlgo->setCurrentIndex(MVNote::algoType == MVNote::PREDEF ? MVNote::preDefAlgoIndex : MVNote::userAlgoIndex);

    for(int i=0;i<NB_OSC;i++)
    {
        MVNote::oscData[i].pan = settings.value(QString("osc%1pan").arg(i),0).toFloat();
        MVNote::oscData[i].amp = settings.value(QString("osc%1amp").arg(i),0.5).toFloat();
        MVNote::oscData[i].freqratio = settings.value(QString("osc%1freqratio").arg(i),1).toFloat();
        MVNote::oscData[i].detune = settings.value(QString("osc%1detune").arg(i),1).toFloat();
        MVNote::oscData[i].randomize = settings.value(QString("osc%1random").arg(i),0).toFloat();
        MVNote::oscData[i].iMod = settings.value(QString("osc%1iMod").arg(i),0.5).toFloat();
        MVNote::oscData[i].tremAmp = settings.value(QString("osc%1tremAmp").arg(i),0).toFloat();
        MVNote::oscData[i].tremFreq = settings.value(QString("osc%1tremFreq").arg(i),5).toFloat();
        MVNote::oscData[i].tremShape = settings.value(QString("osc%1tremShape").arg(i),0).toInt();

        MVNote::oscData[i].vibAmp = settings.value(QString("osc%1vibAmp").arg(i),0).toFloat();
        MVNote::oscData[i].vibFreq = settings.value(QString("osc%1vibFreq").arg(i),5).toFloat();
        MVNote::oscData[i].vibShape = settings.value(QString("osc%1vibShape").arg(i),0).toInt();

        for (int j=0;j<NB_ENV_PTS;j++)
        {
            MVAmpEnvelope::envAmpData[i].points[j].frame = msecToFrames(settings.value(QString("envAmp%1%2frame").arg(i).arg(j),960).toInt());
            if(MVAmpEnvelope::envAmpData[i].points[j].frame == 0)
                MVAmpEnvelope::envAmpData[i].points[j].frame = MVPlayer::getSampleRate()/1000; // Ancients presets
            if(j>0)
            {
                MVFreqEnvelope::envFreqData[i].points[j].frame = msecToFrames(settings.value(QString("envFreq%1%2frame").arg(i).arg(j),960).toInt());
                if(MVFreqEnvelope::envAmpData[i].points[j].frame == 0)
                    MVFreqEnvelope::envAmpData[i].points[j].frame = MVPlayer::getSampleRate()/1000; // Anciens presets
            }
            if(j<NB_ENV_PTS-1)
            {
                MVAmpEnvelope::envAmpData[i].points[j].value = settings.value(QString("envAmp%1%2value").arg(i).arg(j),0.5).toFloat();
                MVFreqEnvelope::envFreqData[i].points[j].value = settings.value(QString("envFreq%1%2value").arg(i).arg(j),1).toFloat();
            }
        }
        MVAmpEnvelope::envAmpData[i].sustainPoint = settings.value(QString("envAmp%1sustainPoint").arg(i),3).toInt();
        MVFreqEnvelope::envFreqData[i].sustainPoint = settings.value(QString("envFreq%1sustainPoint").arg(i),3).toInt();
        MVAmpEnvelope::envAmpData[i].loop = settings.value(QString("envAmp%1loop").arg(i),false).toBool();
        MVFreqEnvelope::envFreqData[i].loop = settings.value(QString("envFreq%1loop").arg(i),false).toBool();
        MVFreqEnvelope::envFreqData[i].used = settings.value(QString("envFreq%1used").arg(i),true).toBool();

        MVAmpEnvelope::envAmpData[i].points[NB_ENV_PTS-1].value = 0.0;
        MVFreqEnvelope::envFreqData[i].points[NB_ENV_PTS-1].value = 1.0;
    }
    MVNote::createAlgo();
    algoDrawer->update();
    updateUIs();

}

void MVFrame::savePreset(QString fileName)
{
    if (! fileName.endsWith((".mvpms")))
        fileName += ".mvpms";
    QSettings settings(fileName,QSettings::NativeFormat);
    settings.setValue("PreDefinedAlgo",MVNote::algoType == MVNote::PREDEF);
    if (MVNote::algoType == MVNote::PREDEF)
        settings.setValue(QString("numAlgo"), MVNote::preDefAlgoIndex);
    else
        settings.setValue(QString("algoDescriptor"), MVNote::userAlgoDescriptor.at(MVNote::userAlgoIndex));

    for(int i=0;i<NB_OSC;i++)
    {
        settings.setValue(QString("osc%1pan").arg(i), MVNote::oscData[i].pan);
        settings.setValue(QString("osc%1amp").arg(i), MVNote::oscData[i].amp);
        settings.setValue(QString("osc%1freqratio").arg(i), MVNote::oscData[i].freqratio);
        settings.setValue(QString("osc%1random").arg(i), MVNote::oscData[i].randomize);
        settings.setValue(QString("osc%1iMod").arg(i), MVNote::oscData[i].iMod);
        settings.setValue(QString("osc%1tremAmp").arg(i), MVNote::oscData[i].tremAmp);
        settings.setValue(QString("osc%1tremFreq").arg(i), MVNote::oscData[i].tremFreq);
        settings.setValue(QString("osc%1tremShape").arg(i), MVNote::oscData[i].tremShape);
        settings.setValue(QString("osc%1vibAmp").arg(i), MVNote::oscData[i].vibAmp);
        settings.setValue(QString("osc%1vibFreq").arg(i), MVNote::oscData[i].vibFreq);
        settings.setValue(QString("osc%1vibShape").arg(i), MVNote::oscData[i].vibShape);

        for (int j=0;j<NB_ENV_PTS;j++)
        {
           settings.setValue(QString("envAmp%1%2frame").arg(i).arg(j), framesTomsec(MVAmpEnvelope::envAmpData[i].points[j].frame));
           settings.setValue(QString("envAmp%1%2value").arg(i).arg(j), MVAmpEnvelope::envAmpData[i].points[j].value);
           settings.setValue(QString("envFreq%1%2frame").arg(i).arg(j), framesTomsec(MVFreqEnvelope::envFreqData[i].points[j].frame));
           settings.setValue(QString("envFreq%1%2value").arg(i).arg(j), MVFreqEnvelope::envFreqData[i].points[j].value);
        }
        settings.setValue(QString("envAmp%1sustainPoint").arg(i), MVAmpEnvelope::envAmpData[i].sustainPoint);
        settings.setValue(QString("envFreq%1sustainPoint").arg(i), MVFreqEnvelope::envFreqData[i].sustainPoint);
        settings.setValue(QString("envAmp%1loop").arg(i), MVAmpEnvelope::envAmpData[i].loop);
        settings.setValue(QString("envFreq%1loop").arg(i), MVFreqEnvelope::envFreqData[i].loop);
        settings.setValue(QString("envFreq%1used").arg(i), MVFreqEnvelope::envFreqData[i].used);
    }
}

void MVFrame::setTimeScales()
{
    int maxSustainFrame = 0;
    int maxReleaseFrame = 0;
    int maxFrame = 0;

    for(int i=0;i<NB_OSC;i++)
    {
        maxSustainFrame = qMax(maxSustainFrame, MVAmpEnvelope::sustainLength(i));
        maxReleaseFrame = qMax(maxReleaseFrame, MVAmpEnvelope::releaseLength(i));
        maxSustainFrame = qMax(maxSustainFrame, MVFreqEnvelope::sustainLength(i));
        maxReleaseFrame = qMax(maxReleaseFrame, MVFreqEnvelope::releaseLength(i));
    }
    maxFrame = maxSustainFrame + maxReleaseFrame;
    for(int i=0;i<NB_OSC;i++)
    {
        maxFrame = qMax(maxFrame, MVAmpEnvelope::totalLength(i));
        maxFrame = qMax(maxFrame, MVFreqEnvelope::totalLength(i));
    }

    for(int i=0;i<NB_OSC;i++)
    {
        envAmpUI[i]->getDrawer()->setTimeScale(maxFrame, maxSustainFrame);
        envFreqUI[i]->getDrawer()->setTimeScale(maxFrame, maxSustainFrame);
        envAmpUI[i]->getDrawer()->update();
        envFreqUI[i]->getDrawer()->update();
    }
}

void MVFrame::updateUIs()
{
    cbAlgo->setCurrentIndex(MVNote::algoType == MVNote::PREDEF ? MVNote::preDefAlgoIndex : MVNote::userAlgoIndex);
    algoDrawer->update();
    dialDiapason->update();

    for(int i=0;i<NB_OSC;i++)
    {
        oscUI[i]->updateUI();
        envAmpUI[i]->updateUI();
        envFreqUI[i]->updateUI();
    }
}

void MVFrame::updateAmpEnvelope(int i)
{
    envAmpUI[i]->updateDrawer();
}
void MVFrame::updateFreqEnvelope(int i)
{
    envFreqUI[i]->updateDrawer();
}

void MVFrame::onPlayTest()
{
    Globals::player->startPlay(69, 64);
}

void MVFrame::onStopTest()
{
    Globals::player->stopPlay(69);
}

void MVFrame::onButtPanic()
{
    Globals::player->stopAllNotes();
}
