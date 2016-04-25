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

#include "MVFilter.h"

#include "MVDefines.h"
#include "MVNote.h"
#include "MVFreqEnvelope.h"

#include <math.h>

MVFilter::MVFilter(const unsigned int sr, const float & fn, const FilterData &fd) : fNote(fn), filterData(fd)
{
    sampleRate = sr;
    dLIn = new float[NB_SAMPLES];
    dLOut = new float[NB_SAMPLES];

    freqEnvelope = new MVFreqEnvelope(*Globals::presetManager->getCurrentPreset()->getFilterEnvData(), -1);
    iDL = 0;
    iDL = 0;
    for(int i=0;i<NB_SAMPLES;i++)
    {
        dLIn[i] = 0.0;
        dLOut[i] = 0.0;
    }
    f = oldF = fNote;
    oldQ = filterData.q;
    oldType = filterData.type;
    computeParams();
}

void MVFilter::reset()
{
    freqEnvelope->reset();
    for(int i=0;i<NB_SAMPLES;i++)
    {
        dLIn[i] = 0.0;
        dLOut[i] = 0.0;
    }
    computeParams();
}

void MVFilter::startRelease()
{
    if(Globals::presetManager->getCurrentPreset()->getFilterEnvData()->used)
        freqEnvelope->startRelease();
}

MVFilter::~MVFilter()
{
    delete freqEnvelope;
    delete[] dLIn;
    delete[] dLOut;
}

void MVFilter::computeParams()
{
    float w0 = M2PI * f / (float)sampleRate;

    float cosw0 = cos(w0);
    float sinw0 = sin(w0);
    float alpha = sinw0/(2.0*Globals::presetManager->getCurrentPreset()->getFilterData()->q);
    oldQ = filterData.q;

    switch(Globals::presetManager->getCurrentPreset()->getFilterData()->type)
    {
        case LP :
            b[0] = (1.0 - cosw0)/2.0;
            b[1] = 1.0 - cosw0;
            b[2] = (1.0 - cosw0)/2.0;
            a[0] = 1.0 + alpha;
            a[1] = -2.0 * cosw0;
            a[2] = 1.0 - alpha;
        break;
        case HP :
            b[0] = (1.0 + cosw0)/2.0;
            b[1] = -1.0 - cosw0;
            b[2] = (1.0 + cosw0)/2.0;
            a[0] = 1.0 + alpha;
            a[1] = -2.0 * cosw0;
            a[2] = 1.0 - alpha;
        break;
        case BP :
            b[0] = sinw0/2.0;
            b[1] = 0.0;
            b[2] = -sinw0/2.0;
            a[0] = 1.0 + alpha;
            a[1] = -2.0 * cosw0;
            a[2] = 1.0 - alpha;
        break;
        case NOTCH :
            b[0] = 1.0;
            b[1] = -2.0 * cosw0;
            b[2] = 1.0;
            a[0] = 1.0 + alpha;
            a[1] = -2.0 * cosw0;
            a[2] = 1.0 - alpha;
        break;
    }
}


