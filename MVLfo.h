#ifndef MVLFO_H
#define MVLFO_H

#include "MVDefines.h"
#include "MVPlayer.h"

#include <math.h>

class MVLfo
{
public:
    MVLfo(const float & a, const float & f);
    inline void reset(){dt = 0; t = 0.0; phi = 0;}
    inline float value() const {return v;}

    inline void next()
    {
        phi -= (freq-oldFreq)*M2PI*t;
        t = (float)dt/MVPlayer::getSampleRate();
        v = amp * sin(M2PI*freq*t + phi);
        oldFreq = freq;
        dt++;
    }

private :
    int dt;
    float t;
    float v;
    float phi;
    const float & amp;
    const float & freq;
    float oldFreq;
};

#endif // MVLFO_H
