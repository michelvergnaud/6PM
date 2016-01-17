#ifndef MVOSCILLATOR_H
#define MVOSCILLATOR_H


#include "MVAmpEnvelope.h"
#include "MVFreqEnvelope.h"
#include "MVNote.h"
#include "MVLfo.h"

class MVOscillator
{

public:
    MVOscillator(int i, MVNote * n, const float & noteT);
    ~MVOscillator();

    inline int getNum() const {return num;}

    inline void reset(bool bAll)
    {
        freqEnvelope->reset();
        ampEnvelope->reset();
        if(bAll)
        {
            oldFreq = freq = note->getFreq() * freqEnvelope->nextFrame() * data.freqratio * MVNote::pitchBend  * data.detune;
            phi = 0.0f;
            v = 0.0f;
            lfoTremolo->reset();
            lfoVibrato->reset();
        }
    }
    inline void startRelease()
    {
        ampEnvelope->startRelease();
        freqEnvelope->startRelease();
    }
    inline float value() const {return v ;}
    inline bool isFinished() const{return ampEnvelope->isFinished();}

    inline void next()
    {
        freq = note->getFreq() * MVNote::pitchBend * data.instantFreq;

        if(MVFreqEnvelope::envFreqData[num].used)
            freq *= freqEnvelope->nextFrame();

        if(lfoVibrato->value() != 1.0)
            freq *= pow(2.0f, lfoVibrato->value());

        if(freq != oldFreq)
        {
            phi = fmod(phi+(oldFreq-freq)*M2PI*t,M2PI);
            oldFreq = freq;
        }

        float modulation = 0.0f;
        for(int i=0; i<MVNote::modulatorsData[num].count(); i++)
        {
            int modIndex = MVNote::modulatorsData[num].value(i,-1);
            if(modIndex > -1)
                modulation += data.iMod * note->getOscillator(modIndex).value();
        }        
        v = data.amp * ampEnvelope->nextFrame() * lfoTremolo->value() * sinf(M2PI * freq * t + phi + modulation);


        lfoTremolo->next();
        lfoVibrato->next();
    }

private:
    int num;
    const MVNote::OscillatorData & data;
    MVAmpEnvelope * ampEnvelope;
    MVFreqEnvelope * freqEnvelope;
    MVNote * note;
    MVLfo * lfoTremolo;
    MVLfo * lfoVibrato;
    float v;
    float freq;
    float amp;
    float oldFreq;
    float phi;

    const float & t;
};

#endif // MVOSCILLATOR_H
