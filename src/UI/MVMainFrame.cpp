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

#include "MVMainFrame.h"

#include "MVControlUIManager.h"
#include "Synth/MVPlayer.h"
#include "MVOscillator_UI.h"
#include "MVAmpEnvelope_UI.h"
#include "MVFreqEnvelope_UI.h"

#include "MVFilterEnvDialog.h"
#include "MVDial.h"
#include "Synth/MVFreqEnvelope.h"
#include "MVEnvelopeDrawer.h"
#include "MVPushButton.h"
#include "MVMeter.h"
#include "MVAlgoDrawer.h"
#include "MVAlgoEditor.h"
#include "MVBackground.h"
#include "MVCheckBox.h"
#include "MVDropTitle.h"
#include "Synth/MVControlNames.h"

#include <QLayout>
#include <QSettings>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QMessageBox>
#include <QLineEdit>
#include <QSplitter>


MVMainFrame::MVMainFrame(QWidget *parent) : QSplitter(parent)
{
    Globals::frame = this;
    QSettings settings;

    QWidget * rightPanel = new QWidget();
    setStyleSheet("QLabel{color: yellow}");
    MVBackground::setColors(Qt::darkGray, QColor::fromRgb(144, 128, 128), QColor::fromRgb(50,48,48));

    QGridLayout * mainLayout = new QGridLayout();
    mainLayout->setSpacing(6);
    mainLayout->setContentsMargins(2,2,2,2);
    rightPanel->setLayout(mainLayout);

    MVBackground * leftPanel = new MVBackground();
    QVBoxLayout * leftPanelLayout = new QVBoxLayout();
    leftPanel->setLayout(leftPanelLayout);

    cbMidiMap = new QComboBox(this);
    updateCbMidiMap();

    midiMapButtonsWidget = new MVBackground();
    QGridLayout * midiMapButtonsBox = new QGridLayout;
    midiMapButtonsWidget->setLayout(midiMapButtonsBox);
    midiMapButtonsBox->addWidget(cbMidiMap,0,0,1,2);
    MVDropTitle * midiMapTitle = new MVDropTitle(midiMapButtonsWidget,"Midi map",this, settings.value("midiMapVisible", true).toBool());
    leftPanelLayout->addWidget(midiMapTitle,0);
    leftPanelLayout->addWidget(midiMapButtonsWidget,1);

    butSaveMidiMap = new MVPushButton(tr("Save"), this);
    butSaveMidiMapAs = new MVPushButton(tr("Save as"), this);
    butRenameMidiMap = new MVPushButton(tr("Rename"), this);
    butDeleteMidiMap = new MVPushButton(tr("Delete"), this);
    midiMapButtonsBox->addWidget(butSaveMidiMap,1,0);
    midiMapButtonsBox->addWidget(butSaveMidiMapAs,1,1);
    midiMapButtonsBox->addWidget(butRenameMidiMap,2,0);
    midiMapButtonsBox->addWidget(butDeleteMidiMap,2,1);

    algoButtonsWidget = new MVBackground();
    QGridLayout * algoButtonsLayout = new QGridLayout;
    algoButtonsWidget->setLayout(algoButtonsLayout);
    radioPreDef = new QRadioButton(tr("Pre-defined"),algoButtonsWidget);
    radioUser = new QRadioButton(tr("User"), algoButtonsWidget);
    radioPreDef->setChecked(Globals::presetManager->getCurrentPreset()->isPreDefAlgo());
    radioUser->setChecked( ! radioPreDef->isChecked());
    butEditAlgo = new MVPushButton(tr("Edit"), algoButtonsWidget);
    butEditAlgo->setEnabled(radioUser->isChecked());

    algoButtonsLayout->addWidget(radioPreDef,2,0);
    algoButtonsLayout->addWidget(radioUser,2,1);
    algoButtonsLayout->addWidget(butEditAlgo,3,1);

    cbAlgo = new QComboBox(this);
    if(radioPreDef->isChecked())
    {
        for(int i=0;i<NB_PREDEF_ALGOS;i++)
            cbAlgo->addItem(tr("Algorithm %1").arg(i+1));
        cbAlgo->setCurrentIndex(Globals::presetManager->getPredefAlgoIndex());
    }
    else
    {
        for(int i=0;i<MVPresetManager::userAlgoDescriptors.size();i++)
            cbAlgo->addItem(tr("User algo %1").arg(i+1));
        cbAlgo->setCurrentIndex(Globals::presetManager->getUserAlgoIndex());
    }
    algoDrawer = new MVAlgoDrawer();
    algoDrawer->setMinimumHeight(100);

    algoEditorDialog = new MVAlgoEditorDialog(this);
    algoEditorDialog->hide();

    cbBank = new QComboBox(this);
    QFont font = cbBank->font();
    font.setPointSize(10);
    cbBank->setFont(font);
    cbBank->addItems(Globals::presetManager->banksList());
    butNewBank = new MVPushButton(tr("New"), this);
    butRenameBank = new MVPushButton(tr("Rename"), this);

    cbPreset = new QComboBox(this);
    font = cbPreset->font();
    font.setPointSize(15);
    cbPreset->setFont(font);

    MVBackground * presetButtonsWidget = new MVBackground();
    QGridLayout * presetButtonsBox = new QGridLayout;
    presetButtonsBox->setContentsMargins(0,0,0,0);
    presetButtonsWidget->setLayout(presetButtonsBox);

    MVPushButton::setColors(Qt::black, Qt::darkGray);
    butSavePreset = new MVPushButton(tr("Save"), this);
    butSavePresetAs = new MVPushButton(tr("Save as"),this);
    butReloadPreset= new MVPushButton(tr("Reload"),this);
    butRenamePreset = new MVPushButton(tr("Rename"),this);
    butDeletePreset = new MVPushButton(tr("Delete"),this);

    presetButtonsBox->setColumnStretch(0,10);
    presetButtonsBox->setColumnStretch(1,10);
    presetButtonsBox->setColumnStretch(2,0);
    presetButtonsBox->setColumnStretch(3,10);
    presetButtonsBox->setColumnStretch(4,20);

    presetButtonsBox->addWidget(cbPreset,0,0,1,5);

    presetButtonsBox->addWidget(butSavePreset,1,0,1,2);
    presetButtonsBox->addWidget(butSavePresetAs,1,3,1,2);
    presetButtonsBox->addWidget(butReloadPreset,2,1,1,3);
    presetButtonsBox->addWidget(butRenamePreset,3,0,1,2);
    presetButtonsBox->addWidget(butDeletePreset,3,3,1,2);

    MVDropTitle * algoTitle = new MVDropTitle(algoButtonsWidget,tr("Algorithm"),this, settings.value("algoVisible", true).toBool());
    leftPanelLayout->addWidget(algoTitle,0);
    algoButtonsLayout->addWidget(cbAlgo,0,0,1,2);
    algoButtonsLayout->addWidget(algoDrawer,1,0,1,2);
    leftPanelLayout->addWidget(algoButtonsWidget,10);

    bankButtonsWidget = new MVBackground();
    QGridLayout * bankButtonsLayout = new QGridLayout();
    bankButtonsWidget->setLayout(bankButtonsLayout);
    bankButtonsLayout->addWidget(cbBank,0,0,1,2);
    bankButtonsLayout->addWidget(butNewBank,1,0);
    bankButtonsLayout->addWidget(butRenameBank,1,1);

    leftPanelLayout->addWidget(new QWidget(),4);//padding

    MVDropTitle * bankTitle = new MVDropTitle(bankButtonsWidget,tr("Bank"),this, settings.value("bankVisible", true).toBool());
    leftPanelLayout->addWidget(bankTitle,0);
    leftPanelLayout->addWidget(bankButtonsWidget,1);

    QLabel * labelPreset  = new QLabel(tr("Preset"));
    leftPanelLayout->addWidget(labelPreset,0);
    leftPanelLayout->addWidget(presetButtonsWidget,10);

    meterL = new MVMeter(Globals::player->getMaxBufL());
    meterL->setMinimumWidth(10);
    meterR = new MVMeter(Globals::player->getMaxBufR());
    meterR->setMinimumWidth(10);

    MVBackground * buttsWidget = new MVBackground();
    QVBoxLayout * buttsLayout = new QVBoxLayout(buttsWidget);
    buttsLayout->setContentsMargins(1,2,1,2);
    buttsLayout->setSpacing(0);
    buttsWidget->setLayout(buttsLayout);
    dialDiapason = new MVDial(buttsWidget,440,"");
    dialDiapason->setAssignable(false);

    dialDiapason->setMinimum(420);
    dialDiapason->setMaximum(460);

    dialDiapason->setValue(Globals::player->getDiapason());
    dialDiapason->setLabel(QString::number(Globals::player->getDiapason()));
    dialDiapason->setLabelColor(Qt::green);
    dialDiapason->setMinimumSize(40,40);

    butTest = new MVPushButton("Test",this);

    butPanic = new QPushButton(QIcon(":/images/stop"),"");
    butPanic->setStyleSheet("background-color: rgb(255, 64, 64);");

    buttsLayout->addWidget(new QLabel("Diapason"),1);
    buttsLayout->addWidget(dialDiapason,2);
    buttsLayout->addWidget(butTest);

    buttsLayout->addWidget(new QWidget, 10);
    buttsLayout->addWidget(new QLabel(tr("Panic")),1);
    buttsLayout->addWidget(butPanic,3);

    QWidget* volWidget = new QWidget(this);
    QHBoxLayout * volLayout = new QHBoxLayout();
    volWidget->setLayout(volLayout);
    volSlider = new MVSlider(this,50,CTRN_mainVolume);
    volSlider->setMinimum(0);
    volSlider->setMaximum(100);
    volSlider->setValue(settings.value("mainVolume", 50 ).toInt());

    volLayout->setContentsMargins(2,2,2,2);
    volLayout->addWidget(buttsWidget,3);
    volLayout->addWidget(meterL,1);
    volLayout->addWidget(meterR,1);
    volLayout->addWidget(volSlider,4);

    QWidget * filterWidget = new QWidget();
    QVBoxLayout * filterLayout = new QVBoxLayout();
    filterLayout->setContentsMargins(0,0,0,0);
    filterLayout->setSpacing(0);
    filterWidget->setLayout(filterLayout);

    comboFilterType = new QComboBox(this);
    comboFilterType->addItem(tr("Low Pass"));
    comboFilterType->addItem(tr("High Pass"));
    comboFilterType->addItem(tr("Band Pass"));
    comboFilterType->addItem(tr("Notch"));
    cbFilterUsed = new MVCheckBox(this, ":/images/usedOn",":/images/usedOff",CTRN_filterUsedmidiCC);
    cbFilterUsed->setMinimumSize(10,10);

    dialFilterF = new MVDial(this,0,CTRN_filterFRatiomidiCC);
    dialFilterF->setMinimum(-48);
    dialFilterF->setMaximum(+48);
    dialFilterF->setValue(0);

    dialFilterQ = new MVDial(this,0,CTRN_filterQmidiCC);
    dialFilterQ->setMinimum(1);
    dialFilterQ->setMaximum(100);

    radAbs = new QRadioButton("Abs", this);
    radNote = new QRadioButton("Note", this);
    radAbs->setContentsMargins(0,0,0,0);
    radNote->setContentsMargins(0,0,0,0);
    radNote->setChecked(true);
    QPalette palette;
    palette.setColor(QPalette::Foreground,Qt::yellow);
    radAbs->setPalette(palette);
    radNote->setPalette(palette);
    font = radAbs->font();
    font.setPointSize(6);
    radAbs->setFont(font);
    radNote->setFont(font);

    QWidget * filterDialsWidget = new MVBackground();
    QGridLayout * filterDialsLayout = new QGridLayout();
    filterDialsLayout->setSpacing(0);
    filterDialsLayout->setContentsMargins(0,0,0,0);
    filterDialsWidget->setLayout(filterDialsLayout);

    filterDialsLayout->addWidget(new QLabel(tr("Filter")),0,0);
    filterDialsLayout->addWidget(cbFilterUsed,0,1,Qt::AlignRight);
    filterDialsLayout->addWidget(dialFilterF,1,0);
    filterDialsLayout->addWidget(dialFilterQ,1,1);
    filterDialsLayout->addWidget(comboFilterType,2,0,1,2);
    filterDialsLayout->addWidget(radAbs,3,0);
    filterDialsLayout->addWidget(radNote,3,1);

    butShowFilterEnvDialog = new MVPushButton(tr("Envelope"), this);
    filterLayout->addWidget(filterDialsWidget,3);
    filterLayout->addWidget(butShowFilterEnvDialog,1);
    filterEnvDialog = new MVFilterEnvDialog(this);
    filterEnvDialog->setWindowTitle(tr("6PM Filter Freq Envelope"));
    volLayout->addWidget(filterWidget);

    leftPanelLayout->addWidget(volWidget,0);

    MVEnvelopeDrawer::envelopeClipboard.typeEnv = -1;
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

        oscUI[i] = new MVOscillator_UI(*Globals::presetManager->getCurrentPreset()->getOscData(i),
                                       *Globals::presetManager->getCurrentPreset()->getTremData(i),
                                       *Globals::presetManager->getCurrentPreset()->getVibData(i), i);
        envAmpUI[i] = new MVAmpEnvelope_UI(*Globals::presetManager->getCurrentPreset()->getOscAmpEnvData(i), i);
        envFreqUI[i] = new MVFreqEnvelope_UI(*Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(i),i);

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

    addWidget(leftPanel);
    addWidget(rightPanel);

    setChildrenCollapsible(false);
    QString lastMidiMap = settings.value("lastMidiMap").toString();
    Globals::controlmanager->loadMidiMap(lastMidiMap);
    cbMidiMap->setCurrentText(Globals::controlmanager->getCurrentMidiMap());

    cbBank->setCurrentText(Globals::presetManager->getCurrentBank());
    cbPreset->addItems(Globals::presetManager->presetsList());
    cbPreset->setCurrentText(Globals::presetManager->getCurrentPreset()->getName());
    setPresetModified(false);

    connect(cbMidiMap,SIGNAL(currentTextChanged(QString)),this,SLOT(midiMapChanged(QString)));
    connect(butEditAlgo,SIGNAL(clicked()),this,SLOT(editAlgo()));

    connect(cbBank,SIGNAL(currentTextChanged(QString)),this,SLOT(bankChanged(QString)));
    connect(volSlider,SIGNAL(valueChanged(int)),this,SLOT(mainVolChanged(int)));

    connect(butSaveMidiMap,SIGNAL(clicked()),this,SLOT(onButtSaveMidiMap()));
    connect(butSaveMidiMapAs,SIGNAL(clicked()),this,SLOT(onButtSaveMidiMapAs()));
    connect(butRenameMidiMap,SIGNAL(clicked()),this,SLOT(onButtRenameMidiMap()));
    connect(butDeleteMidiMap,SIGNAL(clicked()),this,SLOT(onButtDeleteMidiMap()));

    connect(butSavePreset,SIGNAL(clicked()),this,SLOT(onButtSavePreset()));
    connect(butSavePresetAs,SIGNAL(clicked()),this,SLOT(onButtSavePresetAs()));
    connect(butReloadPreset,SIGNAL(clicked()),this,SLOT(onButtReloadPreset()));
    connect(butRenamePreset,SIGNAL(clicked()),this,SLOT(onButtRenamePreset()));
    connect(butDeletePreset,SIGNAL(clicked()),this,SLOT(onButtDeletePreset()));

    connect(butNewBank,SIGNAL(clicked()),this,SLOT(onButtNewBank()));
    connect(butRenameBank,SIGNAL(clicked()),this,SLOT(onButtRenameBank()));

    connect(butPanic,SIGNAL(pressed()),this,SLOT(onButtPanic()));
    connect(dialDiapason,SIGNAL(valueChanged(int)),this,SLOT(diapasonChanged(int)));
    connect(butTest,SIGNAL(pressed()),this,SLOT(onPlayTest()));
    connect(butTest,SIGNAL(released()),this,SLOT(onStopTest()));

    connectWidgets();
    updateUIs();
}

MVMainFrame::~MVMainFrame()
{
    delete[] oscUI;
    delete[] envAmpUI;
    delete[] envFreqUI;
}

void MVMainFrame::connectWidgets()
{
    connect(cbAlgo,SIGNAL(currentIndexChanged(int)),this,SLOT(algoChanged(int)));
    connect(radioPreDef,SIGNAL(toggled(bool)),this,SLOT(algoTypeChanged()));
    connect(cbPreset,SIGNAL(currentTextChanged(QString)),this,SLOT(presetChanged(QString)));
    connect(cbFilterUsed,SIGNAL(toggled(bool)),this,SLOT(cbFilterToggled(bool)));
    connect(comboFilterType,SIGNAL(currentIndexChanged(int)),this,SLOT(filterTypeChanged(int)));
    connect(dialFilterF,SIGNAL(valueChanged(int)),this,SLOT(filterFChanged(int)));
    connect(dialFilterQ,SIGNAL(valueChanged(int)),this,SLOT(filterQChanged(int)));
    connect(radAbs, SIGNAL(toggled(bool)),this,SLOT(setAbsF(bool)));
    connect(butShowFilterEnvDialog,SIGNAL(clicked()),this,SLOT(showFilterEnvDialog()));

}
void MVMainFrame::disconnectWidgets()
{
    disconnect(cbAlgo,SIGNAL(currentIndexChanged(int)),this,SLOT(algoChanged(int)));
    disconnect(radioPreDef,SIGNAL(toggled(bool)),this,SLOT(algoTypeChanged()));
    disconnect(cbPreset,SIGNAL(currentTextChanged(QString)),this,SLOT(presetChanged(QString)));
    disconnect(cbFilterUsed,SIGNAL(toggled(bool)),this,SLOT(cbFilterToggled(bool)));
    disconnect(comboFilterType,SIGNAL(currentIndexChanged(int)),this,SLOT(filterTypeChanged(int)));
    disconnect(dialFilterF,SIGNAL(valueChanged(int)),this,SLOT(filterFChanged(int)));
    disconnect(dialFilterQ,SIGNAL(valueChanged(int)),this,SLOT(filterQChanged(int)));
    disconnect(radAbs, SIGNAL(toggled(bool)),this,SLOT(setAbsF(bool)));
    disconnect(butShowFilterEnvDialog,SIGNAL(clicked()),this,SLOT(showFilterEnvDialog()));

}

void MVMainFrame::saveSettings()
{
    QSettings settings;
    settings.setValue("mainVolume", volSlider->value());
    settings.setValue("midiMapVisible",midiMapButtonsWidget->isVisible());
    settings.setValue("algoVisible",algoButtonsWidget->isVisible());
    settings.setValue("bankVisible",bankButtonsWidget->isVisible());
}

void MVMainFrame::editAlgo()
{
    saveUserAlgoIndex = Globals::presetManager->getUserAlgoIndex();
    algoEditorDialog->init(saveUserAlgoIndex);
    algoEditorDialog->show();

}
void MVMainFrame::cancelEditAlgo()
{
    MVNote::createAlgo(MVPresetManager::userAlgoDescriptors.at(saveUserAlgoIndex));
    algoDrawer->update();
}
void MVMainFrame::updateAlgos()
{
    updateCbAlgo();
    updateUIs();
}
void MVMainFrame::algoChanged(int a)
{
    bool bPredef = radioPreDef->isChecked();
    Globals::presetManager->getCurrentPreset()->setPredefAlgo(bPredef);

    if(bPredef)
    {
        algoEditorDialog->hide();
        Globals::presetManager->getCurrentPreset()->setPredefAlgoIndex(a);
        MVNote::createAlgo();
    }
    else // user
    {
        Globals::presetManager->setUserAlgoIndex(a);
        MVNote::createAlgo();
        algoEditorDialog->init(a);
    }
    algoDrawer->update();
    Globals::presetManager->setModified(true);
    setPresetModified(true);
    updateUIs();
}

void MVMainFrame::updateCbAlgo()
{
    cbAlgo->disconnect(this);
    cbAlgo->clear();
    if(Globals::presetManager->getCurrentPreset()->isPreDefAlgo())
    {
        for(int i=0;i<NB_PREDEF_ALGOS;i++)
            cbAlgo->addItem(tr("Algorithm %1").arg(i+1));
        cbAlgo->setCurrentIndex(Globals::presetManager->getCurrentPreset()->getPredefAlgoIndex());
    }
    else
    {
        for(int i=0;i<MVPresetManager::userAlgoDescriptors.size();i++)
            cbAlgo->addItem(tr("User algo %1").arg(i+1));
        cbAlgo->setCurrentIndex(Globals::presetManager->getUserAlgoIndex());
    }
    connect(cbAlgo,SIGNAL(currentIndexChanged(int)),this,SLOT(algoChanged(int)));
}

void MVMainFrame::algoTypeChanged()
{
    bool b = radioPreDef->isChecked();
    if(b != Globals::presetManager->getCurrentPreset()->isPreDefAlgo())
    {
        Globals::presetManager->getCurrentPreset()->setPredefAlgo(b);
        butEditAlgo->setEnabled( ! b);
        if( b )
            algoEditorDialog->hide();
        MVNote::createAlgo();
        updateCbAlgo();
        algoDrawer->update();
        setPresetModified(true);
        Globals::presetManager->setModified(true);
        updateUIs();
    }
}

void MVMainFrame::mainVolChanged(int v)
{
    MVPlayer::mainVolume = v/100.0;
}

void MVMainFrame::diapasonChanged(int n)
{
    int newDiapason = n;
    Globals::player->setDiapason(newDiapason);
    dialDiapason->setLabel(QString::number(newDiapason));
}
void MVMainFrame::cbFilterToggled(bool b)
{
    bool old = Globals::presetManager->getCurrentPreset()->getFilterData()->used;
    Globals::presetManager->getCurrentPreset()->getFilterData()->used = b;
    dialFilterF->setEnabled(b);
    dialFilterQ->setEnabled(b);
    comboFilterType->setEnabled(b);
    radAbs->setEnabled(b);
    radNote->setEnabled(b);
    setPresetModified(old != b);
    Globals::presetManager->setModified(old != b);
}
void MVMainFrame::filterTypeChanged(int t)
{
    int old = Globals::presetManager->getCurrentPreset()->getFilterData()->type;
    Globals::presetManager->getCurrentPreset()->getFilterData()->type = t;
    Globals::frame->setPresetModified(old != t);
    Globals::presetManager->setModified(old != t);
}
void MVMainFrame::filterFChanged(int n)
{
    float fr = pow(TWELFTH_ROOT2,(float)n);
    Globals::presetManager->getCurrentPreset()->getFilterData()->fRatio = fr;
    if(Globals::presetManager->getCurrentPreset()->getFilterEnvData()->absolute)
    {
        QString label;
        float f = 1000 * pow(2,(float)dialFilterF->value()/12.0);
        if(f>=1000)
            label = QString::number(f/1000,'f',1) + "k";
        else
            label = QString::number(f,'f',1);
        dialFilterF->setLabel("F\n"+label);
    }
    else
        dialFilterF->setLabel("F\n"+QString::number(dialFilterF->value()));

    filterEnvDialog->getDrawer()->update();
    setPresetModified(true);
    Globals::presetManager->setModified(true);
}
void MVMainFrame::filterQChanged(int n)
{
    float q = (float)n/10.0;
    Globals::presetManager->getCurrentPreset()->getFilterData()->q = q;
    dialFilterQ->setLabel("Q\n"+QString::number(q,'f',1));
    setPresetModified(true);
    Globals::presetManager->setModified(true);
}

void MVMainFrame::setAbsF(bool b)
{
    bool old = Globals::presetManager->getCurrentPreset()->getFilterEnvData()->absolute;
    Globals::presetManager->getCurrentPreset()->getFilterEnvData()->absolute = b;
    filterEnvDialog->getDrawer()->update();
    updateUIs();
    setPresetModified(old != b);
    Globals::presetManager->setModified(old != b);
}

void MVMainFrame::midiMapChanged(QString newName)
{
    if(Globals::controlmanager->loadMidiMap(newName))
    {
        QSettings settings;
        settings.setValue("lastMidiMap",newName);
    }
    cbMidiMap->setEditable(false);
}
void MVMainFrame::showFilterEnvDialog()
{
    filterEnvDialog->setVisible(! filterEnvDialog->isVisible());
}
void MVMainFrame::bankChanged(int n)
{
    if(Globals::presetManager->setCurrentBank(n))
    {
        cbPreset->disconnect();
        updateCbPresets();
        cbPreset->setCurrentIndex(-1);
        connect(cbPreset,SIGNAL(currentTextChanged(QString)),this,SLOT(presetChanged(QString)));
    }
}
void MVMainFrame::bankChanged(QString name)
{
    if(Globals::presetManager->setCurrentBank(name))
    {
        updateCbPresets();
        cbPreset->setCurrentIndex(-1);
    }
}

void MVMainFrame::presetChanged(QString presetName)
{
    if(Globals::presetManager->setCurrentPreset(presetName))
    {
        bool bPredef = Globals::presetManager->getCurrentPreset()->isPreDefAlgo();
        cbPreset->setEditable(false);
        radioPreDef->setChecked(bPredef);
        radioUser->setChecked( ! bPredef);
        MVNote::createAlgo();
        updateCbAlgo();
        int indAlgo = bPredef ? Globals::presetManager->getPredefAlgoIndex() :
                                Globals::presetManager->getUserAlgoIndex();
        cbAlgo->setCurrentIndex(indAlgo);
        algoDrawer->update();

        setPresetModified(Globals::presetManager->getCurrentPreset()->isModified());
        updateUIs();
    }
    else
        if(cbPreset->currentIndex()>-1)
            std::cout << "Unknown preset" << std::endl;
}

void MVMainFrame::updateBanksAndPresets()
{
    updateCbBank();
    updateCbPresets();
}
void MVMainFrame::updateCbBank()
{
    QString bank = cbBank->currentText();
    cbBank->disconnect();
    cbBank->clear();
    cbBank->addItems(Globals::presetManager->banksList());
    cbBank->setCurrentText(bank);
    connect(cbBank,SIGNAL(currentTextChanged(QString)),this,SLOT(bankChanged(QString)));
}

void MVMainFrame::updateCbPresets()
{
    cbPreset->disconnect();
    cbPreset->clear();
    cbPreset->addItems(Globals::presetManager->presetsList());
    connect(cbPreset,SIGNAL(currentTextChanged(QString)),this,SLOT(presetChanged(QString)));
}

void MVMainFrame::updateCbMidiMap()
{
    cbMidiMap->clear();
    QDir dir(Globals::userMidiMapsDir);
    QStringList nameFilter;
    nameFilter << QString("*") + MIDIMAP_FILE_EXT;
    QList<QFileInfo>  midiMapList = dir.entryInfoList( nameFilter, QDir::Files );

    cbMidiMap->addItem(tr("None"));
    foreach(QFileInfo f, midiMapList)
    {
        QString item = f.baseName();
        cbMidiMap->addItem(item);
    }
}

void MVMainFrame::onButtSaveMidiMap()
{
    QString fileName = Globals::userMidiMapsDir + "/" + cbMidiMap->currentText() + ".mvmm";
    QString fileNameBak = fileName+ ".bak";
    QFile::copy(fileName, fileNameBak);
    Globals::controlmanager->saveMidiMap(fileName);
}
void MVMainFrame::onButtSaveMidiMapAs()
{
    cbMidiMap->disconnect();
    editSender = butSaveMidiMapAs;
    cbMidiMap->setEditable(true);
    connect(cbMidiMap->lineEdit(),SIGNAL(returnPressed()),this,SLOT(midiMapReturnPressed()));
    connect(cbMidiMap->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingMidiMapFinished()));
    cbMidiMap->setFocus();
}
void MVMainFrame::onButtRenameMidiMap()
{
    cbMidiMap->disconnect();
    editSender = butRenameMidiMap;
    oldFileName = Globals::userMidiMapsDir + "/" + cbMidiMap->currentText() + ".mvmm";
    cbMidiMap->setEditable(true);
    connect(cbMidiMap->lineEdit(),SIGNAL(returnPressed()),this,SLOT(midiMapReturnPressed()));
    connect(cbMidiMap->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingMidiMapFinished()));
    cbMidiMap->setFocus();
}
void MVMainFrame::onButtDeleteMidiMap()
{
    QString fileName = Globals::userMidiMapsDir + "/" + cbMidiMap->currentText() + ".mvmm";
    if (QMessageBox::question(this, tr("Delete midi map"),tr("Are you sure ?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
    {
        QFile::copy(fileName, fileName + ".bak");
        QFile::remove(fileName);
        updateCbMidiMap();
    }
}
void MVMainFrame::midiMapReturnPressed()
{
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
            QMessageBox::warning(NULL, tr("Rename midi map"),tr("Impossible to rename file"));
            cbMidiMap->setCurrentText(QFileInfo(oldFileName).baseName());
        }
    }
    else
    {
        bool saveIt = true;
        if ( QFile::exists(newFileName))
        {
            saveIt = QMessageBox::question(this, tr("Save midi map"),tr("Replace file ?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes;
        }
        if(saveIt)
        {
            Globals::controlmanager->saveMidiMap(newFileName);
            updateCbMidiMap();
            cbMidiMap->setCurrentText(newName);
        }
    }
}

void MVMainFrame::editingMidiMapFinished()
{
    if(cbMidiMap->lineEdit())
        cbMidiMap->lineEdit()->disconnect();
    cbMidiMap->setEditable(false);
    connect(cbMidiMap,SIGNAL(currentTextChanged(QString)),this,SLOT(midiMapChanged(QString)));
}

void MVMainFrame::setPresetModified(bool b)
{
    butSavePreset->setEnabled(b);
    butReloadPreset->setEnabled(b);
}

void MVMainFrame::onButtSavePreset()
{
    if(Globals::presetManager->saveCurrentPreset())
        setPresetModified(false);
}

void MVMainFrame::onButtSavePresetAs()
{
    editSender = butSavePresetAs;
    oldName = cbPreset->currentText();
    cbPreset->disconnect();
    cbPreset->setEditable(true);
    connect(cbPreset->lineEdit(),SIGNAL(returnPressed()),this,SLOT(presetReturnPressed()));
    connect(cbPreset->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingPresetFinished()));
    cbPreset->setFocus();
}
void MVMainFrame::onButtReloadPreset()
{
    Globals::presetManager->reloadCurrentPreset();
    bool bPredef = Globals::presetManager->getCurrentPreset()->isPreDefAlgo();
    radioPreDef->setChecked(bPredef);
    int indAlgo = bPredef ? Globals::presetManager->getPredefAlgoIndex() :
                            Globals::presetManager->getUserAlgoIndex();
    cbAlgo->setCurrentIndex(indAlgo);
    updateUIs();
    setPresetModified(false);
}

void MVMainFrame::keyReleaseEvent(QKeyEvent * e)
{
    if(e->key() == Qt::Key_Escape)
    {
        if(cbPreset->isEditable() )
            editingPresetFinished();
        else
        if(cbBank->isEditable() )
            editingBankFinished();
        else
        if(cbMidiMap->isEditable() )
            editingMidiMapFinished();
    }
}

void MVMainFrame::onButtRenamePreset()
{
    editSender = butRenamePreset;
    oldName = cbPreset->currentText();
    cbPreset->disconnect();
    cbPreset->setEditable(true);

    connect(cbPreset->lineEdit(),SIGNAL(returnPressed()),this,SLOT(presetReturnPressed()));
    connect(cbPreset->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingPresetFinished()));

    cbPreset->setFocus();
}
void MVMainFrame::onButtDeletePreset()
{
    if (QMessageBox::question(this, "Delete preset","Are you sure ?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
    {
        int index = cbPreset->currentIndex();
        Globals::presetManager->deleteCurrentPreset();
        updateCbPresets();
        cbPreset->setCurrentIndex(index);
    }
}

void MVMainFrame::presetReturnPressed()
{
    newName = cbPreset->currentText();
    cbPreset->setEditable(false);

    QString newFileName = Globals::userPresetsDir + "/" + Globals::presetManager->getCurrentBank() + "/" + newName + PRESET_FILE_EXT;

    if(QFile::exists(newFileName) &&
            QMessageBox::question(this,tr("6PM preset name"),tr("Overwrite preset ") + newName + " ?", QMessageBox::Yes,QMessageBox::No) == QMessageBox::No)
    {
        cbPreset->setCurrentText(oldName);
        return;
    }

    if(editSender == butRenamePreset)
    {
        if (Globals::presetManager->renameCurrentPreset(newName))
        {
            updateCbPresets();
            cbPreset->setCurrentText(newName);
        }
        else
        {
            QMessageBox::warning(NULL, tr("Rename preset"),tr("Impossible to rename file"));
            cbPreset->setCurrentText(oldName);
        }
    }
    else // save as
    {        
        if( Globals::presetManager->saveCurrentPresetAs(newName))
        {
            updateCbPresets();
            cbPreset->setCurrentText(newName);
            setPresetModified(false);
        }
        else
        {
            QMessageBox::warning(NULL, tr("Save preset as"),tr("Impossible to create file"));
            cbPreset->setCurrentText(oldName);
        }
    }
}

void MVMainFrame::editingPresetFinished()
{
    cbPreset->setEditable(false);
    cbPreset->setCurrentText(newName);
    connect(cbPreset,SIGNAL(currentTextChanged(QString)),this,SLOT(presetChanged(QString)));
}

void MVMainFrame::onButtNewBank()
{
    editSender = butNewBank;
    cbBank->disconnect();
    cbBank->setEditable(true);
    connect(cbBank->lineEdit(),SIGNAL(returnPressed()),this,SLOT(bankReturnPressed()));
    connect(cbBank->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingBankFinished()));
    cbBank->setFocus();
}
void MVMainFrame::onButtRenameBank()
{
    editSender = butRenameBank;
    oldName = cbBank->currentText();
    cbBank->disconnect();
    cbBank->setEditable(true);

    connect(cbBank->lineEdit(),SIGNAL(returnPressed()),this,SLOT(bankReturnPressed()));
    connect(cbBank->lineEdit(),SIGNAL(editingFinished()),this,SLOT(editingBankFinished()));
    cbBank->setFocus();
}
void MVMainFrame::bankReturnPressed()
{
    newName = cbBank->currentText();
    if(editSender == butNewBank)
    {
        if(Globals::presetManager->createNewBank(newName))
        {
            updateCbBank();
            cbBank->setCurrentText(newName);
            updateCbPresets();
        }
        else
        {
            QMessageBox::critical(NULL, tr("Create bank"),tr("Impossible to create directory"));
            cbBank->setCurrentText(oldName);
        }

    }
    else // Rename
    {
        if( Globals::presetManager->renameCurrentBank(newName))
        {
            updateCbBank();
            cbBank->setCurrentText(newName);
        }
        else
        {
            QMessageBox::critical(NULL, tr("Rename bank"),tr("Impossible to rename directory"));
            cbBank->setCurrentText(oldName);
        }
    }
}

void MVMainFrame::editingBankFinished()
{
    if(cbBank->lineEdit())
        cbBank->lineEdit()->disconnect();
    cbBank->setEditable(false);
    connect(cbBank,SIGNAL(currentTextChanged(QString)),this,SLOT(bankChanged(QString)));
}

void MVMainFrame::addPreset(QString presetName)
{
    updateCbPresets();
    cbPreset->setCurrentText(presetName);
}
// Called from Midi Control Change
void MVMainFrame::loadPreset(int n)
{
    if(n>=0 && n < cbPreset->count())
        cbPreset->setCurrentIndex(n);
    else
        std::cout << "Preset number out of range" << std::endl;
}
// Called from Midi Control Change
void MVMainFrame::bankSelect(int n)
{
    if(n>=0 && n < cbBank->count())
        cbBank->setCurrentIndex(n);
    else
        std::cout << "Bank number out of range" << std::endl;
}

void MVMainFrame::setTimeScales()
{
    int maxSustainFrame = 0;
    int maxReleaseFrame = 0;
    int maxFrame = 0;

    for(int i=0;i<NB_OSC;i++)
    {
        maxSustainFrame = qMax(maxSustainFrame, MVAmpEnvelope::sustainLength(*Globals::presetManager->getCurrentPreset()->getOscAmpEnvData(i)));
        maxReleaseFrame = qMax(maxReleaseFrame, MVAmpEnvelope::releaseLength(*Globals::presetManager->getCurrentPreset()->getOscAmpEnvData(i)));
        maxSustainFrame = qMax(maxSustainFrame, MVFreqEnvelope::sustainLength(*Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(i)));
        maxReleaseFrame = qMax(maxReleaseFrame, MVFreqEnvelope::releaseLength(*Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(i)));
    }
    maxSustainFrame = qMax(maxSustainFrame, MVFreqEnvelope::sustainLength(*Globals::presetManager->getCurrentPreset()->getFilterEnvData()));
    maxReleaseFrame = qMax(maxReleaseFrame, MVFreqEnvelope::releaseLength(*Globals::presetManager->getCurrentPreset()->getFilterEnvData()));

    maxFrame = maxSustainFrame + maxReleaseFrame;
    for(int i=0;i<NB_OSC;i++)
    {
        maxFrame = qMax(maxFrame, MVAmpEnvelope::totalLength(*Globals::presetManager->getCurrentPreset()->getOscAmpEnvData(i)));
        maxFrame = qMax(maxFrame, MVFreqEnvelope::totalLength(*Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(i)));
    }
    maxFrame = qMax(maxFrame, MVFreqEnvelope::totalLength(*Globals::presetManager->getCurrentPreset()->getFilterEnvData()));
    maxFrame = qMax(maxFrame, MVFreqEnvelope::totalLength(*Globals::presetManager->getCurrentPreset()->getFilterEnvData()));


    for(int i=0;i<NB_OSC;i++)
    {
        envAmpUI[i]->getDrawer()->setTimeScale(maxFrame, maxSustainFrame);
        envFreqUI[i]->getDrawer()->setTimeScale(maxFrame, maxSustainFrame);
        envAmpUI[i]->getDrawer()->update();
        envFreqUI[i]->getDrawer()->update();
    }
    filterEnvDialog->getDrawer()->setTimeScale(maxFrame, maxSustainFrame);
    filterEnvDialog->getDrawer()->update();
}

void MVMainFrame::updateUIs()
{
    disconnectWidgets();
    bool bPredef = Globals::presetManager->getCurrentPreset()->isPreDefAlgo();
    int indAlgo = bPredef ? Globals::presetManager->getPredefAlgoIndex() :
                            Globals::presetManager->getUserAlgoIndex();
    cbAlgo->setCurrentIndex(indAlgo);
    algoDrawer->update();
    dialDiapason->update();

    for(int i=0;i<NB_OSC;i++)
    {
        oscUI[i]->updateUI();
        envAmpUI[i]->updateUI();
        envFreqUI[i]->updateUI();
    }
    cbFilterUsed->setChecked(Globals::presetManager->getCurrentPreset()->getFilterData()->used);
    comboFilterType->setCurrentIndex(Globals::presetManager->getCurrentPreset()->getFilterData()->type);
    radAbs->setChecked(Globals::presetManager->getCurrentPreset()->getFilterEnvData()->absolute);
    radNote->setChecked( ! Globals::presetManager->getCurrentPreset()->getFilterEnvData()->absolute);

    dialFilterF->setValue(round((log(Globals::presetManager->getCurrentPreset()->getFilterData()->fRatio)/log(TWELFTH_ROOT2))));

    if(Globals::presetManager->getCurrentPreset()->getFilterEnvData()->absolute)
    {
        QString label;
        float f = 1000 * pow(2,(float)dialFilterF->value()/12.0);
        if(f>=1000)
            label = QString::number(f/1000,'f',1) + "k";
        else
            label = QString::number(f,'f',1);
        dialFilterF->setLabel("F\n"+label);
    }
    else
        dialFilterF->setLabel("F\n"+QString::number(dialFilterF->value()));

    dialFilterQ->setValue(Globals::presetManager->getCurrentPreset()->getFilterData()->q*10.0);
    dialFilterQ->setLabel("Q\n"+QString::number(Globals::presetManager->getCurrentPreset()->getFilterData()->q,'f',1));
    cbFilterUsed->update();
    dialFilterF->update();
    dialFilterQ->update();

    comboFilterType->setEnabled(Globals::presetManager->getCurrentPreset()->getFilterData()->used);
    dialFilterF->setEnabled(Globals::presetManager->getCurrentPreset()->getFilterData()->used);
    dialFilterQ->setEnabled(Globals::presetManager->getCurrentPreset()->getFilterData()->used);
    radAbs->setEnabled(Globals::presetManager->getCurrentPreset()->getFilterData()->used);
    radNote->setEnabled(Globals::presetManager->getCurrentPreset()->getFilterData()->used);

    filterEnvDialog->updateUI();
    setTimeScales();
    connectWidgets();
}

void MVMainFrame::updateAmpEnvelope(int i)
{
    envAmpUI[i]->updateUI();
    envAmpUI[i]->updateLabels();
    envAmpUI[i]->updateDrawer();
}
void MVMainFrame::updateFreqEnvelope(int i)
{
    if(i >=0)
    {
        envFreqUI[i]->updateUI();
        envFreqUI[i]->updateLabels();
        envFreqUI[i]->updateDrawer();
    }
    else
        filterEnvDialog->updateUI();
}

void MVMainFrame::onPlayTest()
{
    Globals::player->startPlay(69, 64);
}

void MVMainFrame::onStopTest()
{
    Globals::player->stopPlay(69);
}

void MVMainFrame::onButtPanic()
{
    Globals::player->stopAllNotes();
}
