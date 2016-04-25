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

#include "MVFreqEnvelope.h"
#include <iostream>

MVFreqEnvelope::MVFreqEnvelope(const FreqEnvelopeData &ed, int n) : data(ed)
{
    numOsc = n;
    reset();
    bFinished = true;
}

void MVFreqEnvelope::reset()
{
    bSustain = false;
    bFinished = false;
    v = data.points[0].value;
    nextP = 1;
    nbSteps = 0;
}

float MVFreqEnvelope::nextFrame()
{
    if (bFinished)
        v = data.points[NB_ENV_PTS-1].value;
    else
    {
        if(bSustain)
            v = data.points[data.sustainPoint].value;
        else
        {
            unsigned int n = data.points[nextP].frame;
            if(n > nbSteps)
                v += (float)(data.points[nextP].value - v) / (n - nbSteps);
        }
    }
    nbSteps++;
    if(nbSteps >= data.points[nextP].frame && ! bSustain)
    {
       if(nextP == data.sustainPoint)
            bSustain = true;
       else
            nextP++;
       if (nextP == NB_ENV_PTS)
       {
            if(data.loop)
            {
                v = data.points[0].value;
                nextP = 1;
            }
            else
                bFinished = true;
       }
       nbSteps = 0;
    }

    return v;
}

void MVFreqEnvelope::startRelease()
{
    if(data.sustainPoint > -1)
    {
        bSustain = false;
        nextP = data.sustainPoint+1;
        nbSteps = 0;
    }
}
int MVFreqEnvelope::totalLength(const FreqEnvelopeData &env)
{
    int n = 0;
    for(int j=0;j<NB_ENV_PTS;j++)
        n += env.points[j].frame;
    return n;
}

int MVFreqEnvelope::sustainLength(const FreqEnvelopeData &env)
{
    int n = 0;
    for(int j=0;j<=env.sustainPoint;j++)
        n += env.points[j].frame;
    return n;
}

int MVFreqEnvelope::releaseLength(const FreqEnvelopeData &env)
{
    int n = 0;
    if(env.sustainPoint > -1)
        for(int j=env.sustainPoint+1;j<NB_ENV_PTS;j++)
            n += env.points[j].frame;
    return n;
}

float * MVFreqEnvelope::timeValues;
int MVFreqEnvelope::nbTimeValues = 64;
