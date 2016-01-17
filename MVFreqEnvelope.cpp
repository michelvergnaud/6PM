/*
    6PM
     Copyright (c) 2015, Michel Vergnaud

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

MVFreqEnvelope::MVFreqEnvelope(const EnvelopeData & ed, int & n) : MVAmpEnvelope(ed, 0, n)//, oscData(od)
{
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

int MVFreqEnvelope::totalLength(int i)
{
    int n = 0;
    for(int j=1;j<NB_ENV_PTS;j++)
        n += envFreqData[i].points[j].frame;
    return n;
}
int MVFreqEnvelope::sustainLength(int i)
{
    int n = 0;
    for(int j=1;j<=envFreqData[i].sustainPoint;j++)
        n += envFreqData[i].points[j].frame;
    return n;
}
int MVFreqEnvelope::releaseLength(int i)
{
    int n = 0;
    if(envFreqData[i].sustainPoint > -1)
        for(int j=envFreqData[i].sustainPoint+1;j<NB_ENV_PTS;j++)
            n += envFreqData[i].points[j].frame;
    return n;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MVAmpEnvelope::EnvelopeData MVFreqEnvelope::envFreqData[NB_OSC] =
{
    (Point){0,1.0},(Point){22000,1.0},(Point){44000,1.0},(Point){33000,1.0},(Point){960,1.0},(Point){480,1.0},(int)3,(bool)false,(bool)true,
    (Point){0,1.0},(Point){22000,1.0},(Point){44000,1.0},(Point){33000,1.0},(Point){960,1.0},(Point){480,1.0},(int)3,(bool)false,(bool)true,
    (Point){0,1.0},(Point){22000,1.0},(Point){44000,1.0},(Point){33000,1.0},(Point){960,1.0},(Point){480,1.0},(int)3,(bool)false,(bool)true,
    (Point){0,1.0},(Point){22000,1.0},(Point){44000,1.0},(Point){33000,1.0},(Point){960,1.0},(Point){480,1.0},(int)3,(bool)false,(bool)true,
    (Point){0,1.0},(Point){22000,1.0},(Point){44000,1.0},(Point){33000,1.0},(Point){960,1.0},(Point){480,1.0},(int)3,(bool)false,(bool)true,
    (Point){0,1.0},(Point){22000,1.0},(Point){44000,1.0},(Point){33000,1.0},(Point){960,1.0},(Point){480,1.0},(int)3,(bool)false,(bool)true
 };
