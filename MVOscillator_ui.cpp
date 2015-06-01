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

#include <math.h>

#include <QVBoxLayout>


MVOscillator_UI::MVOscillator_UI(MVNote::OscillatorData & od, int n ) : data(od)
{
    numOsc = n;
    data = od;
    sliderPan = new MVSlider();
    sliderPan->setOrientation(Qt::Horizontal);

    sliderPan->setMinimum(0);
    sliderPan->setMaximum(100);
    dialAmp = new MVDial(this,50);
    dialAmp->setMinimum(0);
    dialAmp->setMaximum(100);

    dialFreqRatio = new MVDial(this,0);
    dialFreqRatio->setMinimum(-24); // -2 octaves by 1 semitone step
    dialFreqRatio->setMaximum(+24); // +2 octaves

    dialDetune = new MVDial(this,0); // 100 cents, 1 semitone
    dialDetune->setMinimum(-100);
    dialDetune->setMaximum(100);

    dialIMod = new MVDial(this,50);
    dialIMod->setMinimum(0);
    dialIMod->setMaximum(200);

    dialTremoloAmp = new MVDial(this,0);
    dialTremoloAmp->setMinimum(0);
    dialTremoloAmp->setMaximum(100);

    dialTremoloFreq = new MVDial(this,50);
    dialTremoloFreq->setMinimum(10);
    dialTremoloFreq->setMaximum(100);

    dialVibratoAmp = new MVDial(this,0);
    dialVibratoAmp->setMinimum(0);
    dialVibratoAmp->setMaximum(100);

    dialVibratoFreq = new MVDial(this,50);
    dialVibratoFreq->setMinimum(10);
    dialVibratoFreq->setMaximum(100);

    QVBoxLayout * layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(2);
    this->setLayout(layout);
    layout->addWidget(sliderPan);
    layout->addWidget(dialAmp);
    layout->addWidget(dialFreqRatio);
    layout->addWidget(dialDetune);
    layout->addWidget(dialTremoloAmp);
    layout->addWidget(dialTremoloFreq);
    layout->addWidget(dialVibratoAmp);
    layout->addWidget(dialVibratoFreq);
    layout->addWidget(dialIMod);

    updateUI();

    connect(sliderPan, SIGNAL(valueChanged(int)),this,SLOT(setPan(int)));
    connect(dialAmp, SIGNAL(valueChanged(int)),this,SLOT(setAmp(int)));
    connect(dialFreqRatio, SIGNAL(valueChanged(int)),this,SLOT(setFreqRatio(int)));
    connect(dialDetune, SIGNAL(valueChanged(int)),this,SLOT(setDetune(int)));
    connect(dialIMod, SIGNAL(valueChanged(int)),this,SLOT(setIndMod(int)));
    connect(dialTremoloAmp, SIGNAL(valueChanged(int)),this,SLOT(setTremAmp(int)));
    connect(dialTremoloFreq, SIGNAL(valueChanged(int)),this,SLOT(setTremFreq(int)));
    connect(dialVibratoAmp, SIGNAL(valueChanged(int)),this,SLOT(setVibAmp(int)));
    connect(dialVibratoFreq, SIGNAL(valueChanged(int)),this,SLOT(setVibFreq(int)));
}

void MVOscillator_UI::updateUI()
{
    sliderPan->setEnabled(MVNote::isOp(numOsc));
    dialIMod->setEnabled(MVNote::isModulated(numOsc));
    sliderPan->setValue((data.pan+0.5) * 100);
    dialAmp->setValue(data.amp*100);
    dialFreqRatio->setValue(round((log(data.freqratio)/log(TWELFTH_ROOT2))));
    dialDetune->setValue(round(100*log(data.detune)/log(TWELFTH_ROOT2)));
    dialIMod->setValue(data.iMod * 10);
    dialTremoloAmp->setValue(data.tremAmp*10);
    dialTremoloFreq->setValue(data.tremFreq*10);
    dialVibratoAmp->setValue(data.vibAmp*100);
    dialVibratoFreq->setValue(data.vibFreq*10);
    updateLabels();
}
void MVOscillator_UI::updateLabels()
{
    dialAmp->setLabel("A\n"+QString::number(data.amp,'f',2));
    dialFreqRatio->setLabel("F\n"+QString::number(dialFreqRatio->value()));
    dialDetune->setLabel("detune\n"+QString::number(dialDetune->value()));
    dialIMod->setLabel("i mod\n"+QString::number(data.iMod,'f',2));
    dialTremoloAmp->setLabel("trem A\n"+QString::number(data.tremAmp,'f',2));
    dialTremoloFreq->setLabel("trem F\n"+QString::number(data.tremFreq,'f',2));
    dialVibratoAmp->setLabel("vib A\n"+QString::number(data.vibAmp,'f',2));
    dialVibratoFreq->setLabel("vib F\n"+QString::number(data.vibFreq,'f',2));
}
void MVOscillator_UI::setPan(int v)
{
    data.pan = (float)(v-50.0)/100.0;
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
    updateLabels();
    Globals::frame->updateFreqEnvelope(numOsc);
}
void MVOscillator_UI::setDetune(int v)
{
    data.detune = pow(TWELFTH_ROOT2, (float)v/100.0f);
    updateLabels();
    Globals::frame->updateFreqEnvelope(numOsc);
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
