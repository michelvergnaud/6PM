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

#ifndef MVFILTER_H
#define MVFILTER_H

#include <jack/systemdeps.h>

#define NB_SAMPLES 3

#include "MVFreqEnvelope.h"
#include "MVPlayer.h"

class MVFilter
{
public:
    MVFilter(const unsigned int, const float&, const FilterData &);
    ~MVFilter();
    enum FilterType {LP, HP,BP,NOTCH};

    void reset();
    void startRelease();
    inline float computeOut(float paramIn)
    {
        if(Globals::presetManager->getCurrentPreset()->getFilterEnvData()->absolute)
            f = 1000;
        else
            f = fNote;
        f *= filterData.fRatio;

        if(Globals::presetManager->getCurrentPreset()->getFilterEnvData()->used)
            f *= freqEnvelope->nextFrame();

        if(f>Globals::player->getMaxFreq())
            f=Globals::player->getMaxFreq();
        if(f != oldF || filterData.q != oldQ || filterData.type != oldType)
        {
            computeParams();
            oldF = f;
            oldQ = filterData.q;
            oldType = filterData.type;
        }
        int j;
        dLIn[iDL] = paramIn;
        float yn = 0.0;
        for(int i=0;i<NB_SAMPLES;i++)
        {
            if(iDL - i < 0)
                j = i - NB_SAMPLES;
            else
                j = i;
            yn += (b[i]/a[0]) * dLIn[iDL - j];
        }
        for(int i=1;i<NB_SAMPLES;i++)
        {
            if(iDL - i < 0)
                j = i - NB_SAMPLES;
            else
                j = i;
            yn -= (a[i]/a[0]) * dLOut[iDL - j];
        }
        dLOut[iDL] = yn;
        if (iDL < NB_SAMPLES - 1)
            iDL++;
        else
            iDL = 0;

        return yn;
    }

private :
    int iDL;
    float * dLIn;
    float * dLOut;
    float a[NB_SAMPLES];
    float b[NB_SAMPLES];
    float f, oldF, oldQ;
    int oldType;
    void computeParams();
    unsigned int sampleRate;
    MVFreqEnvelope * freqEnvelope;

    const float & fNote;
    const FilterData & filterData;
};

#endif // MVFILTER_H
