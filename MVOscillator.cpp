#include "MVOscillator.h"

#include "MVDefines.h"

MVOscillator::MVOscillator(int i, MVNote * n, const float & noteT) : data(MVNote::oscData[i]), t(noteT)
{
    num = i;
    note = n;
    amp = note->getAmp();
    v = 0.0f;
    phi = 0.0f;
    oldFreq = freq = note->getFreq() * data.freqratio * data.detune;
    ampEnvelope = new MVAmpEnvelope(MVAmpEnvelope::envAmpData[i], note->getAmp(), num);
    freqEnvelope = new MVFreqEnvelope(MVFreqEnvelope::envFreqData[i], num);
    lfoTremolo = new MVLfo(data.tremAmp, data.tremFreq,data.tremShape,MVLfo::TREMOLO);
    lfoVibrato = new MVLfo(data.vibAmp, data.vibFreq,data.vibShape,MVLfo::VIBRATO);
}

MVOscillator::~MVOscillator()
{
    delete ampEnvelope;
    delete freqEnvelope;
    delete lfoTremolo;
    delete lfoVibrato;
}
