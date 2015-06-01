#include "MVLfo.h"

MVLfo::MVLfo(const float &a, const float &f) : amp(a),freq(f)
{
    dt = 0;
    t = 0.0;
    phi = 0.0;
    oldFreq = f;
    v = 0;
}
