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

#include "MVOscillator_ui.h"

#include "MVDial.h"
#include "MVNote.h"
#include "MVGlobals.h"
#include "MVFrame.h"
#include "MVLfo.h"
#include "MVBackground.h"

#include <math.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>


MVOscillator_UI::MVOscillator_UI(MVNote::OscillatorData & od, int n ) : data(od)
{
    numOsc = n;
    data = od;
    sliderPan = new MVSlider();
    sliderPan->setOrientation(Qt::Horizontal);

    sliderPan->setMinimum(0);
    sliderPan->setMaximum(100);
    sliderPan->setDefaultValue(50);
    dialAmp = new MVDial(this,50);
    dialAmp->setMinimum(0);
    dialAmp->setMaximum(100);

    dialFreqRatio = new MVDial(this,0);
    dialFreqRatio->setMinimum(-24); // -2 octaves by 1 semitone step
    dialFreqRatio->setMaximum(+24); // +2 octaves

    dialDetune = new MVDial(this,0); // 100 cents, 1 semitone
    dialDetune->setMinimum(-100);
    dialDetune->setMaximum(100);

    dialRandom = new MVDial(this,0);
    dialRandom->setMinimum(0);
    dialRandom->setMaximum(200);

    dialIMod = new MVDial(this,50);
    dialIMod->setMinimum(0);
    dialIMod->setMaximum(200);

    dialTremoloAmp = new MVDial(this,0);
    dialTremoloAmp->setMinimum(0);
    dialTremoloAmp->setMaximum(10);

    dialTremoloFreq = new MVDial(this,50);
    dialTremoloFreq->setMinimum(1);
    dialTremoloFreq->setMaximum(100);

    dialTremoloShape = new MVDial(this,0);
    dialTremoloShape->setMinimum(0);
    dialTremoloShape->setMaximum(3);

    dialVibratoAmp = new MVDial(this,0);
    dialVibratoAmp->setMinimum(0);
    dialVibratoAmp->setMaximum(100);

    dialVibratoFreq = new MVDial(this,50);
    dialVibratoFreq->setMinimum(1);
    dialVibratoFreq->setMaximum(100);

    dialVibratoShape = new MVDial(this,0);
    dialVibratoShape->setMinimum(0);
    dialVibratoShape->setMaximum(3);

    QPalette pal(Qt::yellow,QColor::fromRgb(32,16,16));
    setPalette(pal);
    setAutoFillBackground(true);
    QVBoxLayout * layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    this->setLayout(layout);

    setStyleSheet("QGroupBox \
                   {color: yellow; border: 1px solid #602020;\
                   background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #0F0A0A, stop: 1 #402020);\
                   margin-top: 2ex;}\
                   QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center;\
                   background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1,stop: 0 #201010, stop: 1 #100000)}\
                  ");

    QVBoxLayout * ampVBox = new QVBoxLayout();
    ampVBox->setContentsMargins(0,0,0,0);
    ampVBox->setSpacing(0);
    QGroupBox * ampWidget = new QGroupBox("Output");
    ampWidget->setLayout(ampVBox);
    ampVBox->addWidget(sliderPan);
    ampVBox->addWidget(dialAmp);
    layout->addWidget(ampWidget,4);

    QGridLayout * freqGrid = new QGridLayout();
    freqGrid->setContentsMargins(0,0,0,0);
    freqGrid->setSpacing(0);
    QGroupBox * freqWidget = new QGroupBox("Freq");
    freqWidget->setLayout(freqGrid);
    freqGrid->addWidget(dialFreqRatio,0,0);
    freqGrid->addWidget(dialDetune,0,1);
    freqGrid->addWidget(dialRandom,1,0);
    layout->addWidget(freqWidget,4);

    QGridLayout * tremGrid = new QGridLayout();
    tremGrid->setContentsMargins(0,0,0,0);
    tremGrid->setSpacing(0);
    QGroupBox * tremWidget = new QGroupBox("Tremolo");
    tremWidget->setLayout(tremGrid);
    tremGrid->addWidget(dialTremoloAmp,0,0);
    tremGrid->addWidget(dialTremoloFreq,0,1);
    tremGrid->addWidget(dialTremoloShape,1,0);
    layout->addWidget(tremWidget,4);

    QGridLayout * vibGrid = new QGridLayout();
    vibGrid->setContentsMargins(0,0,0,0);
    vibGrid->setSpacing(0);
    QGroupBox * vibWidget = new QGroupBox("Vibrato");
    vibWidget->setLayout(vibGrid);
    vibGrid->addWidget(dialVibratoAmp,0,0);
    vibGrid->addWidget(dialVibratoFreq,0,1);
    vibGrid->addWidget(dialVibratoShape,1,0);
    layout->addWidget(vibWidget,4);

    QVBoxLayout * modVBox = new QVBoxLayout();
    modVBox->setContentsMargins(0,0,0,0);
    modVBox->setSpacing(0);
    QGroupBox * modWidget = new QGroupBox("Modulation");
    modWidget->setLayout(modVBox);
    modVBox->addWidget(dialIMod);
    layout->addWidget(modWidget,3);

    updateUI();
    computeFreq();

    connect(sliderPan, SIGNAL(valueChanged(int)),this,SLOT(setPan(int)));
    connect(dialAmp, SIGNAL(valueChanged(int)),this,SLOT(setAmp(int)));
    connect(dialFreqRatio, SIGNAL(valueChanged(int)),this,SLOT(setFreqRatio(int)));
    connect(dialDetune, SIGNAL(valueChanged(int)),this,SLOT(setDetune(int)));
    connect(dialRandom, SIGNAL(valueChanged(int)),this,SLOT(setRandom(int)));
    connect(dialIMod, SIGNAL(valueChanged(int)),this,SLOT(setIndMod(int)));
    connect(dialTremoloAmp, SIGNAL(valueChanged(int)),this,SLOT(setTremAmp(int)));
    connect(dialTremoloFreq, SIGNAL(valueChanged(int)),this,SLOT(setTremFreq(int)));
    connect(dialTremoloShape, SIGNAL(valueChanged(int)),this,SLOT(setTremShape(int)));
    connect(dialVibratoAmp, SIGNAL(valueChanged(int)),this,SLOT(setVibAmp(int)));
    connect(dialVibratoFreq, SIGNAL(valueChanged(int)),this,SLOT(setVibFreq(int)));
    connect(dialVibratoShape, SIGNAL(valueChanged(int)),this,SLOT(setVibShape(int)));

}

void MVOscillator_UI::updateUI()
{
    sliderPan->setEnabled(MVNote::isOp(numOsc));
    dialIMod->setEnabled(MVNote::isModulated(numOsc));
    sliderPan->setValue((data.pan+0.5) * 100);
    dialAmp->setValue(data.amp*100);
    dialFreqRatio->setValue(round((log(data.freqratio)/log(TWELFTH_ROOT2))));
    dialDetune->setValue(round(100*log(data.detune)/log(TWELFTH_ROOT2)));
    dialRandom->setValue(data.randomize*1000);
    dialIMod->setValue(data.iMod * 10);
    dialTremoloAmp->setValue(data.tremAmp*10);
    dialTremoloFreq->setValue(data.tremFreq*10);
    dialTremoloShape->setValue(data.tremShape);
    dialVibratoAmp->setValue(data.vibAmp*100);
    dialVibratoFreq->setValue(data.vibFreq*10);
    dialVibratoShape->setValue(data.vibShape);

    updateLabels();
}
void MVOscillator_UI::updateLabels()
{
    dialAmp->setLabel(QString::number(data.amp,'f',2));
    dialFreqRatio->setLabel("F\n"+QString::number(dialFreqRatio->value()));
    dialDetune->setLabel("det\n"+QString::number(dialDetune->value()));
    dialRandom->setLabel("rand\n"+QString::number(dialRandom->value()));
    dialIMod->setLabel(QString::number(data.iMod,'f',2));
    dialTremoloAmp->setLabel("A\n"+QString::number(data.tremAmp,'f',2));
    dialTremoloFreq->setLabel("F\n"+QString::number(data.tremFreq,'f',2));
    dialTremoloShape->setLabel(MVLfo::waveShapeLabels[data.tremShape]);
    dialVibratoAmp->setLabel("A\n"+QString::number(data.vibAmp,'f',2));
    dialVibratoFreq->setLabel("F\n"+QString::number(data.vibFreq,'f',2));
    dialVibratoShape->setLabel(MVLfo::waveShapeLabels[data.vibShape]);
}
void MVOscillator_UI::computeFreq()
{
    data.instantFreq = data.freqratio * data.detune;
}

void MVOscillator_UI::setPan(int v)
{
    data.pan = ((float)v-50.0)/100.0;
    updateLabels();
}

void MVOscillator_UI::setAmp(int v)
{
    data.amp = (float)v/100.0;
    updateLabels();
    Globals::frame->updateAmpEnvelope(numOsc);
}

void MVOscillator_UI::setFreqRatio(int v)
{
    data.freqratio = pow(TWELFTH_ROOT2,(float)v);
    computeFreq();
    updateLabels();
    Globals::frame->updateFreqEnvelope(numOsc);
}
void MVOscillator_UI::setDetune(int v)
{
    data.detune = pow(TWELFTH_ROOT2, (float)v/100.0f);
    computeFreq();
    updateLabels();
    Globals::frame->updateFreqEnvelope(numOsc);
}
void MVOscillator_UI::setRandom(int v)
{
    data.randomize = v/1000.0f;
    updateLabels();
}

void MVOscillator_UI::setIndMod(int v)
{
    data.iMod = (float)v/10;
    updateLabels();
}
void MVOscillator_UI::setTremAmp(int v)
{
    data.tremAmp = (float)v/10.0;
    updateLabels();
}

void MVOscillator_UI::setTremFreq(int v)
{
    data.tremFreq = (float)v/10.0;
    updateLabels();
}
void MVOscillator_UI::setTremShape(int v)
{
    data.tremShape = v;
    updateLabels();
}

void MVOscillator_UI::setVibAmp(int v)
{
    data.vibAmp = (float)v/100.0;
    updateLabels();
}

void MVOscillator_UI::setVibFreq(int v)
{
    data.vibFreq = (float)v/10.0;
    updateLabels();
}
void MVOscillator_UI::setVibShape(int v)
{
    data.vibShape = v;
    updateLabels();
}
