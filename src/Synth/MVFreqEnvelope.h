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

#ifndef MVFREQENVELOPE_H
#define MVFREQENVELOPE_H

#include "MVUtils.h"
#include "MVGlobals.h"
#include "MVDefines.h"
#include "MVPresetManager.h"
//#include "MVStaticData.h"

class MVFreqEnvelope
{
public:

    MVFreqEnvelope(const FreqEnvelopeData & , int);

    inline int getNumOsc(){return numOsc;}
    inline bool isFinished() const {return bFinished;}
    inline float value() const {return v;}
    void reset();
    void startRelease();
    float nextFrame();

    static inline int getSustainPoint(int i){return Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(i)->sustainPoint;}
    static int totalLength(const FreqEnvelopeData &);
    static int sustainLength(const FreqEnvelopeData &);
    static int releaseLength(const FreqEnvelopeData &);

    static float * timeValues;
    static int nbTimeValues;

private:
    float v;
    unsigned int nbSteps;
    int nextP;
    bool bFinished, bSustain;
    const FreqEnvelopeData & data;
    int numOsc;
 };

#endif // MVFREQENVELOPE_H
