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

#ifndef MVENVELOPE_H
#define MVENVELOPE_H

#include "MVUtils.h"
#include "MVGlobals.h"
#include "MVDefines.h"

class MVAmpEnvelope
{
public:
    struct Point
    {
        unsigned long frame;
        float value;
    };
    struct  EnvelopeData
    {
        Point points[NB_ENV_PTS];
        int sustainPoint;
        bool loop;
    };

    MVAmpEnvelope(const EnvelopeData &, const float & a, int n);

    int getNumOsc(){return numOsc;}

    void reset();
    void startRelease();

    inline bool isFinished() const {return bFinished;}
    inline float value() const {return v;}
    inline void fadeOut() {nbStepsFadeOut = fadeLength;}

    float nextFrame();

    static int totalLength(int);
    static int sustainLength(int);
    static int releaseLength(int);

    static inline int getSustainPoint(int i){return envAmpData[i].sustainPoint;}
    static MVAmpEnvelope::EnvelopeData envAmpData[NB_OSC];

protected:
    float v;

    unsigned int nbSteps;
    int nextP;
    int fadeLength;
    int nbStepsFadeOut;
    bool bFadingOut, bFinished, bSustain, bRestart;

    const EnvelopeData & data;
    const float & amp;
    int numOsc;
};

#endif // MVENVELOPE_H
