#ifndef MVLFO_H
#define MVLFO_H

#include "MVDefines.h"
#include "MVPlayer.h"

#include <math.h>

class MVLfo
{
public:
    enum WaveShape{SINE,SQUARE,SAWUP,SAWDOWN} ;
    enum LfoFunction{TREMOLO,VIBRATO};


    static const QString waveShapeLabels[4];

    MVLfo(const float & a, const float & f, const int & s, LfoFunction func);
    inline void reset(){dt = 0; t = 0.0; phi = 0;tempDt=0;}
    inline float value() const {return v;}

    inline void next()
    {
        switch(waveShape)
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
    const float & amp;
    const float & freq;
    const WaveShape & waveShape;
    float oldFreq;
    int tempDt;
    int nbSamples;

};

#endif // MVLFO_H
