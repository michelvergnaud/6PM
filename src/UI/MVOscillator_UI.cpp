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

#include "MVOscillator_UI.h"

#include "MVDial.h"
#include "Synth/MVNote.h"
#include "MVGlobals.h"
#include "MVMainFrame.h"
#include "Synth/MVLfo.h"
#include "MVPresetManager.h"
#include "Synth/MVControlNames.h"

#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QRadioButton>
#include <QLinearGradient>
#include <QDialog>
#include <math.h>

QLinearGradient * gradient = new QLinearGradient(QPointF(0, 0), QPointF(0,100));
class OscUiBackground : public QWidget
{
public:
    OscUiBackground(QWidget* parent):QWidget(parent)
    {
    }
protected:
  void paintEvent(QPaintEvent *e)
  {
      QWidget::paintEvent(e);
      QPainter painter(this);
      painter.setBackground(*gradient);
      painter.eraseRect(rect());
  }
};


MVOscillator_UI::MVOscillator_UI(OscillatorData &od, LfoData &td, LfoData &vd, int n ) : data(od), tremData(td), vibData(vd)
{
    gradient->setColorAt(0,Qt::black);
    gradient->setColorAt(1,QColor::fromRgb(80,0,0));
    numOsc = n;

    sliderPan = new MVSlider(this,0,QString(CTRN_osc1midiCC_pan).arg(n));
    sliderPan->setOrientation(Qt::Horizontal);
    sliderPan->setMinimum(0);
    sliderPan->setMaximum(100);
    sliderPan->setDefaultValue(50);

    dialAmp = new MVDial(this,50,QString(CTRN_osc1midiCC_amp).arg(n));
    dialAmp->setMinimum(0);
    dialAmp->setMaximum(100);

    dialFreqRatio = new MVDial(this,0,QString(CTRN_osc1midiCC_freqratio).arg(n));
    dialFreqRatio->setMinimum(-48); // -4 octaves by 1 semitone step
    dialFreqRatio->setMaximum(+48); // +4 octaves

    dialDetune = new MVDial(this,0,QString(CTRN_osc1midiCC_detune).arg(n)); // 100 cents, 1 semitone
    dialDetune->setMinimum(-100);
    dialDetune->setMaximum(100);

    dialRandom = new MVDial(this,0,QString(CTRN_osc1midiCC_random).arg(n));
    dialRandom->setMinimum(0);
    dialRandom->setMaximum(200);

    radAbs = new QRadioButton("A", this);
    radNote = new QRadioButton("N", this);
    radAbs->setMinimumSize(4,4);
    radNote->setMinimumSize(4,4);
    radAbs->setContentsMargins(0,0,0,0);
    radNote->setContentsMargins(0,0,0,0);
    radNote->setChecked(true);

    dialIMod = new MVDial(this,50,QString(CTRN_osc1midiCC_iMod).arg(n));
    dialIMod->setMinimum(0);
    dialIMod->setMaximum(200);

    QPalette pal(Qt::yellow,QColor::fromRgb(32,8,8));
    setPalette(pal);
    setAutoFillBackground(true);
    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    this->setLayout(mainLayout);

    QFont labelsFont = font();
    labelsFont.setPointSize(8);

    QGridLayout * ampGrid = new QGridLayout();
    ampGrid->setContentsMargins(1,1,1,1);
    ampGrid->setSpacing(0);
    OscUiBackground * ampWidget = new OscUiBackground(this);
    ampWidget->setLayout(ampGrid);

    ampGrid->addWidget(sliderPan,0,0,1,2);

    QLabel * ampLabel = new QLabel("Amplitude");
    ampLabel->setAlignment(Qt::AlignHCenter);
    ampLabel->setFont(labelsFont);
    ampGrid->addWidget(ampLabel,1,0,1,2);
    ampGrid->addWidget(dialAmp,2,0,1,2);
    mainLayout->addWidget(ampWidget,10);

    tremDialog = new QDialog(this);
    tremDialog->setWindowTitle(tr("Oscillator %1 tremolo").arg(numOsc+1));
    tremDialog->hide();
    QVBoxLayout * tremDialogLayout = new QVBoxLayout(tremDialog);

    dialTremoloAmp = new MVDial(this,0,QString(CTRN_osc1midiCC_tremAmp).arg(n));
    dialTremoloAmp->setMinimum(0);
    dialTremoloAmp->setMaximum(100);

    dialTremoloFreq = new MVDial(this,50,QString(CTRN_osc1midiCC_tremFreq).arg(n));
    dialTremoloFreq->setMinimum(1);
    dialTremoloFreq->setMaximum(100);

    dialTremoloShape = new MVDial(this,0,QString(CTRN_osc1midiCC_tremShape).arg(n));
    dialTremoloShape->setMinimum(0);
    dialTremoloShape->setMaximum(3);
    dialTremoloShape->addPixmap(new QPixmap(":/images/sine"));
    dialTremoloShape->addPixmap(new QPixmap(":/images/rect"));
    dialTremoloShape->addPixmap(new QPixmap(":/images/sawup"));
    dialTremoloShape->addPixmap(new QPixmap(":/images/sawdown"));

    QHBoxLayout * tremWidgetLayout = new QHBoxLayout();
    tremWidgetLayout->setContentsMargins(0,0,0,0);
    tremWidgetLayout->setSpacing(0);
    OscUiBackground * tremWidget = new OscUiBackground(this);
    tremWidget->setLayout(tremWidgetLayout);
    tremWidgetLayout->addWidget(dialTremoloAmp);
    tremWidgetLayout->addWidget(dialTremoloFreq);
    tremWidgetLayout->addWidget(dialTremoloShape);
    tremDialogLayout->addWidget(tremWidget,3);

    tremButton = new MVPushButton("LFO",this);
    ampGrid->addWidget(tremButton,3,1);
    connect(tremButton,SIGNAL(clicked()),this,SLOT(showTremDialog()));

    QGridLayout * freqGrid = new QGridLayout();
    freqGrid->setContentsMargins(0,0,0,0);
    freqGrid->setSpacing(0);
    OscUiBackground * freqWidget = new OscUiBackground(this);
    freqWidget->setLayout(freqGrid);

    QLabel * freqLabel = new QLabel(tr("Frequency"));
    freqLabel->setAlignment(Qt::AlignHCenter);
    freqLabel->setFont(labelsFont);
    freqGrid->addWidget(freqLabel,0,0,1,2);
    freqGrid->addWidget(dialFreqRatio,1,0);
    freqGrid->addWidget(dialDetune,1,1);
    freqGrid->addWidget(dialRandom,2,0);

    QWidget * absNoteWidget = new QWidget(this);
    QVBoxLayout * absNoteLayout = new QVBoxLayout();
    absNoteLayout->setContentsMargins(0,0,0,0);
    absNoteLayout->setSpacing(0);
    absNoteWidget->setLayout(absNoteLayout);

    QPalette palette;
    palette.setColor(QPalette::Foreground,Qt::yellow);
    radAbs->setPalette(palette);
    radNote->setPalette(palette);
    QFont font = radAbs->font();
    font.setPointSize(5);
    radAbs->setFont(font);
    radNote->setFont(font);
    absNoteLayout->addWidget(radAbs);
    absNoteLayout->addWidget(radNote);
    freqGrid->addWidget(absNoteWidget,2,1,Qt::AlignRight);
    mainLayout->addWidget(freqWidget,10,Qt::AlignRight);

    vibDialog = new QDialog(this);
    vibDialog->setWindowTitle(tr("Oscillator %1 vibrato").arg(numOsc+1));
    vibDialog->hide();
    QVBoxLayout * vibDialogLayout = new QVBoxLayout(vibDialog);

    dialVibratoAmp = new MVDial(this,0,QString(CTRN_osc1midiCC_vibAmp).arg(n));
    dialVibratoAmp->setMinimum(0);
    dialVibratoAmp->setMaximum(100);

    dialVibratoFreq = new MVDial(this,50,QString(CTRN_osc1midiCC_vibFreq).arg(n));
    dialVibratoFreq->setMinimum(1);
    dialVibratoFreq->setMaximum(100);

    dialVibratoShape = new MVDial(this,0,QString(CTRN_osc1midiCC_vibShape).arg(n));
    dialVibratoShape->setMinimum(0);
    dialVibratoShape->setMaximum(3);
    dialVibratoShape->addPixmap(new QPixmap(":/images/sine"));
    dialVibratoShape->addPixmap(new QPixmap(":/images/rect"));
    dialVibratoShape->addPixmap(new QPixmap(":/images/sawup"));
    dialVibratoShape->addPixmap(new QPixmap(":/images/sawdown"));

    QHBoxLayout * vibWidgetLayout = new QHBoxLayout();
    vibWidgetLayout->setContentsMargins(0,0,0,0);
    vibWidgetLayout->setSpacing(0);
    OscUiBackground * vibWidget = new OscUiBackground(this);
    vibWidget->setLayout(vibWidgetLayout);
    vibWidgetLayout->addWidget(dialVibratoAmp);
    vibWidgetLayout->addWidget(dialVibratoFreq);
    vibWidgetLayout->addWidget(dialVibratoShape);
    vibDialogLayout->addWidget(vibWidget,3);

    vibButton = new MVPushButton("LFO",this);
    freqGrid->addWidget(vibButton,3,1);
    connect(vibButton,SIGNAL(clicked()),this,SLOT(showVibDialog()));

    QVBoxLayout * modVBox = new QVBoxLayout();
    modVBox->setContentsMargins(0,0,0,0);
    modVBox->setSpacing(0);
    OscUiBackground * modWidget = new OscUiBackground(this);
    modWidget->setLayout(modVBox);

    QLabel * modLabel = new QLabel("Modulation");
    modLabel->setAlignment(Qt::AlignHCenter);
    modLabel->setFont(labelsFont);

    modVBox->addWidget(modLabel,0);
    modVBox->addWidget(dialIMod);

    mainLayout->addWidget(modWidget,0);

    updateUI();
    computeFreq();
    connectWidgets();

}
void MVOscillator_UI::connectWidgets()
{
    connect(sliderPan, SIGNAL(valueChanged(int)),this,SLOT(setPan(int)));
    connect(dialAmp, SIGNAL(valueChanged(int)),this,SLOT(setAmp(int)));
    connect(dialFreqRatio, SIGNAL(valueChanged(int)),this,SLOT(setFreqRatio(int)));
    connect(dialDetune, SIGNAL(valueChanged(int)),this,SLOT(setDetune(int)));
    connect(dialRandom, SIGNAL(valueChanged(int)),this,SLOT(setRandom(int)));
    connect(radAbs, SIGNAL(toggled(bool)),this,SLOT(setAbsF(bool)));
    connect(dialIMod, SIGNAL(valueChanged(int)),this,SLOT(setIndMod(int)));
    connect(dialTremoloAmp, SIGNAL(valueChanged(int)),this,SLOT(setTremAmp(int)));
    connect(dialTremoloFreq, SIGNAL(valueChanged(int)),this,SLOT(setTremFreq(int)));
    connect(dialTremoloShape, SIGNAL(valueChanged(int)),this,SLOT(setTremShape(int)));
    connect(dialVibratoAmp, SIGNAL(valueChanged(int)),this,SLOT(setVibAmp(int)));
    connect(dialVibratoFreq, SIGNAL(valueChanged(int)),this,SLOT(setVibFreq(int)));
    connect(dialVibratoShape, SIGNAL(valueChanged(int)),this,SLOT(setVibShape(int)));
}

void MVOscillator_UI::disconnectWidgets()
{
    sliderPan->disconnect(this);
    dialAmp->disconnect(this);
    dialFreqRatio->disconnect(this);
    dialDetune->disconnect(this);
    dialRandom->disconnect(this);
    radAbs->disconnect(this);
    dialIMod->disconnect(this);
    dialTremoloAmp->disconnect(this);
    dialTremoloFreq->disconnect(this);
    dialTremoloShape->disconnect(this);
    dialVibratoAmp->disconnect(this);
    dialVibratoFreq->disconnect(this);
    dialVibratoShape->disconnect(this);
}
void MVOscillator_UI::updateUI()
{
    disconnectWidgets();
    sliderPan->setEnabled(MVNote::isOp(numOsc));
    dialIMod->setEnabled(MVNote::isModulated(numOsc));
    sliderPan->setValue((data.pan+0.5) * 100);
    dialAmp->setValue(data.amp*100);
    dialFreqRatio->setValue(round((log(data.freqratio)/log(TWELFTH_ROOT2))));
    dialDetune->setValue(round(100*log(data.detune)/log(TWELFTH_ROOT2)));
    dialRandom->setValue(data.randomize*1000);
    dialIMod->setValue(data.iMod * 10);
    tremButton->showLed(tremData.amp > 0.0);
    dialTremoloAmp->setValue(tremData.amp*10);
    dialTremoloFreq->setValue(tremData.freq*10);
    dialTremoloShape->setValue(tremData.shape);
    vibButton->showLed(vibData.amp > 0.0);
    dialVibratoAmp->setValue(vibData.amp*100);
    dialVibratoFreq->setValue(vibData.freq*10);
    dialVibratoShape->setValue(vibData.shape);

    tremButton->showLed(tremData.amp > 0.0);
    vibButton->showLed(vibData.amp > 0.0);

    updateLabels();
    connectWidgets();
}
void MVOscillator_UI::updateLabels()
{
    dialAmp->setLabel(QString::number(data.amp,'f',2));
    if(Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(numOsc)->absolute)
    {
        QString label;
        float f = 1000 * pow(2,(float)dialFreqRatio->value()/12.0);
        if(f>=1000)
            label = QString::number(f/1000,'f',1) + "k";
        else
            label = QString::number(f,'f',1);
        dialFreqRatio->setLabel("F\n"+label);
    }
    else
        dialFreqRatio->setLabel("F\n"+QString::number(dialFreqRatio->value()));

    dialDetune->setLabel("det\n"+QString::number(dialDetune->value()));
    dialRandom->setLabel("rand\n"+QString::number(dialRandom->value()));
    dialIMod->setLabel(QString::number(data.iMod,'f',2));
    dialTremoloAmp->setLabel("A\n"+QString::number(tremData.amp,'f',2));
    dialTremoloFreq->setLabel("F\n"+QString::number(tremData.freq,'f',2));
    dialVibratoAmp->setLabel("A\n"+QString::number(vibData.amp,'f',2));
    dialVibratoFreq->setLabel("F\n"+QString::number(vibData.freq,'f',2));
}
void MVOscillator_UI::computeFreq()
{
    data.instantFreq = data.freqratio * data.detune;
}

void MVOscillator_UI::setPresetModified(bool b)
{
    Globals::presetManager->setModified(true);
    Globals::frame->setPresetModified(b);
}

void MVOscillator_UI::setPan(int v)
{
    data.pan = ((float)v-50.0)/100.0;
    updateLabels();
    setPresetModified(true);
}

void MVOscillator_UI::setAmp(int v)
{
    data.amp = (float)v/100.0;
    updateLabels();
    Globals::frame->updateAmpEnvelope(numOsc);
    setPresetModified(true);
}

void MVOscillator_UI::setFreqRatio(int n)
{
    data.freqratio = pow(TWELFTH_ROOT2,(float)n);
    computeFreq();
    updateLabels();
    Globals::frame->updateFreqEnvelope(numOsc);
    setPresetModified(true);
}
void MVOscillator_UI::setDetune(int v)
{
    data.detune = pow(TWELFTH_ROOT2, (float)v/100.0f);
    computeFreq();
    updateLabels();
    Globals::frame->updateFreqEnvelope(numOsc);
    setPresetModified(true);
}
void MVOscillator_UI::setRandom(int v)
{
    data.randomize = v/1000.0f;
    updateLabels();
    setPresetModified(true);
}
void MVOscillator_UI::setAbsF(bool b)
{
    Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(numOsc)->absolute = b;
    Globals::frame->updateFreqEnvelope(numOsc);
    updateLabels();
    dialFreqRatio->update();
    setPresetModified(true);
}
void MVOscillator_UI::setIndMod(int v)
{
    data.iMod = (float)v/10;
    updateLabels();
    setPresetModified(true);
}
void MVOscillator_UI::setTremAmp(int v)
{
    tremData.amp = (float)v/100.0;
    updateLabels();
    setPresetModified(true);
    tremButton->showLed(tremData.amp > 0.0);
}

void MVOscillator_UI::setTremFreq(int v)
{
    tremData.freq = (float)v/10.0;
    updateLabels();
    setPresetModified(true);
}
void MVOscillator_UI::setTremShape(int v)
{
    tremData.shape = v;
    updateLabels();
    setPresetModified(true);
}

void MVOscillator_UI::setVibAmp(int v)
{
    vibData.amp = (float)v/100.0;
    updateLabels();
    setPresetModified(true);
    vibButton->showLed(vibData.amp > 0.0);
}

void MVOscillator_UI::setVibFreq(int v)
{
    vibData.freq = (float)v/10.0;
    updateLabels();
    setPresetModified(true);
}
void MVOscillator_UI::setVibShape(int v)
{
    vibData.shape = v;
    updateLabels();
    setPresetModified(true);
}
void MVOscillator_UI::showTremDialog()
{
    tremDialog->setVisible( ! tremDialog->isVisible());
}
void MVOscillator_UI::showVibDialog()
{
    vibDialog->setVisible( ! vibDialog->isVisible());
}
