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

#ifndef MVENVELOPE_H
#define MVENVELOPE_H

#include "MVUtils.h"
#include "MVGlobals.h"
#include "MVDefines.h"
#include "MVPresetManager.h"

#include <iostream>

class MVAmpEnvelope
{
public:

    MVAmpEnvelope(AmpEnvelopeData & , const float & a, int n);

    inline int getNumOsc(){return numOsc;}
    inline bool isFinished() const {return bFinished;}
    inline float value() const {return v;}

    void reset();
    void startRelease();
    inline void fadeOut() {nbStepsFadeOut = fadeLength;}
    float nextFrame();

    static inline int getSustainPoint(int i){return Globals::presetManager->getCurrentPreset()->getOscAmpEnvData(i)->sustainPoint;}
    static int totalLength(const AmpEnvelopeData &);
    static int sustainLength(const AmpEnvelopeData &);
    static int releaseLength(const AmpEnvelopeData &);

protected:
    float v;

    unsigned int nbSteps;
    int nextP;
    int fadeLength;
    int nbStepsFadeOut;
    bool bFinished, bSustain, bRestart;

    AmpEnvelopeData & data;
    const float & amp;
    int numOsc;
};

#endif // MVENVELOPE_H
