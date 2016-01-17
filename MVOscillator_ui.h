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

#ifndef MVOSCILLATOR_UI_H
#define MVOSCILLATOR_UI_H

#include "MVNote.h"
#include "MVBackground.h"
#include "MVSlider.h"

class MVDial;

class MVOscillator_UI : public QWidget
{
Q_OBJECT

public:
    MVOscillator_UI(MVNote::OscillatorData & , int);

    void updateUI();

    MVSlider * sliderPan;
    MVDial * dialAmp;
    MVDial * dialFreqRatio;
    MVDial * dialDetune;
    MVDial * dialRandom;
    MVDial * dialIMod;
    MVDial * dialTremoloAmp;
    MVDial * dialTremoloFreq;
    MVDial * dialTremoloShape;
    MVDial * dialVibratoAmp;
    MVDial * dialVibratoFreq;
    MVDial * dialVibratoShape;

private:
    MVNote::OscillatorData & data;
    void updateLabels();
    void computeFreq();
    int numOsc;

private slots:
    void setPan(int);
    void setAmp(int);
    void setFreqRatio(int);
    void setDetune(int);
    void setRandom(int);
    void setIndMod(int);
    void setTremAmp(int);
    void setTremFreq(int);
    void setTremShape(int);
    void setVibAmp(int);
    void setVibFreq(int);
    void setVibShape(int);
};

#endif // MVOSCILLATOR_UI_H
