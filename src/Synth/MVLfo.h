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

#ifndef MVLFO_H
#define MVLFO_H

#include "MVDefines.h"
#include "MVPlayer.h"
#include "MVPreset.h"

#include <math.h>

class MVLfo
{
public:
    enum WaveShape{SINE,SQUARE,SAWUP,SAWDOWN} ;
    enum LfoFunction{TREMOLO,VIBRATO};

    MVLfo( const LfoData &, LfoFunction);

    inline void reset(){dt = 0; t = 0.0; phi = 0;tempDt=0;}
    inline float value() const {return v;}

    inline void next()
    {
        float amp = data.amp;
        float freq = data.freq;
        switch(data.shape)
        {
            case SINE:
                phi -= (freq-oldFreq)*M2PI*t;
                t = (float)dt/MVPlayer::getSampleRate();
                v = lfoFunction==TREMOLO ? 1.0 - amp * (sinf(M2PI*freq*t + phi)/2.0 + 0.5) : amp * sinf(M2PI*freq*t + phi);
                oldFreq = freq;
            break;
            case SQUARE :
                nbSamples = MVPlayer::getSampleRate()/freq;
                tempDt++;
                if(tempDt < nbSamples / 2)
                    v = lfoFunction==TREMOLO ? 1.0 : amp;
                else
                    v = lfoFunction==TREMOLO ? 1.0 - amp : -amp ;
                if (tempDt > nbSamples)
                    tempDt = 0;
            break;
            case SAWUP :
                nbSamples = MVPlayer::getSampleRate()/freq;
                tempDt++;
                if(tempDt > nbSamples)
                    tempDt = 0;
                v = lfoFunction==TREMOLO ? 1.0 - amp*(1.0 - (float)tempDt/(float)nbSamples) : amp*(-1.0 + 2.0*(float)tempDt/(float)nbSamples);
            break;
            case SAWDOWN :
                nbSamples = MVPlayer::getSampleRate()/freq;
                tempDt++;
                if(tempDt > nbSamples)
                    tempDt = 0;
                v = lfoFunction==TREMOLO ? 1.0 - amp*(float)tempDt/(float)nbSamples : amp*(1.0 - 2.0*(float)tempDt/(float)nbSamples);
            break;

        }
        dt++;
    }

private :
    LfoFunction lfoFunction;
    int dt;
    float t;
    float v;
    float phi;
    const LfoData & data;
    float oldFreq;
    int tempDt;
    int nbSamples;
};

#endif // MVLFO_H
