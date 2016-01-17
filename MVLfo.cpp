#include "MVLfo.h"

MVLfo::MVLfo(const float &a, const float &f, const int &s, LfoFunction func) : amp(a),freq(f), waveShape((WaveShape&)s)
{
    lfoFunction = func;
    dt = 0;
    t = 0.0;
    phi = 0.0;
    oldFreq = f;
    tempDt = 0;
    v = 0;
}
const QString MVLfo::waveShapeLabels[4]={"Sine","Square","SawUp","SawDown"};
