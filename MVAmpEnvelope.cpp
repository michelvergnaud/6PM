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

#include "MVAmpEnvelope.h"

#include "MVPlayer.h"

MVAmpEnvelope::MVAmpEnvelope(const EnvelopeData & ed, const float &a, int n) : data(ed), amp(a)
{
    numOsc = n;

    v = 0.0;
    bFinished = true;
    bRestart = true;
    nextP = 0;
    nbSteps = 0;
    fadeLength = MVPlayer::getSampleRate()/1000;
    nbStepsFadeOut = 0;
}

float MVAmpEnvelope::nextFrame()
{
    if(nbStepsFadeOut > 0)
    {
        v -= v/nbStepsFadeOut;
        nbStepsFadeOut--;
        if(nbStepsFadeOut == 0)
            bFinished = true;
        return v;
    }
    if(bRestart)
    {
        bSustain = false;
        bFinished = false;
        nextP = 0;
        nbSteps = 0;
        nbStepsFadeOut = 0;
        bRestart = false;
    }
    if(bFinished)
    {
        v = 0.0;
        return 0;
    }
    if(bSustain && data.sustainPoint > -1)
    {
        v = data.points[data.sustainPoint].value * amp;
       return v;
    }
    unsigned int n = data.points[nextP].frame;
    if(n > nbSteps)
        v += (data.points[nextP].value * amp - v) / (n - nbSteps);
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
                nextP = 0;
            else
                bFinished = true;
       }
       nbSteps = 0;
    }
    return v;
}

void MVAmpEnvelope::reset()
{
   bRestart = true;
   bFinished = false;
}

void MVAmpEnvelope::startRelease()
{
    if(data.sustainPoint > -1)
    {
        bSustain = false;
        nextP = data.sustainPoint+1;
        nbSteps = 0;
    }
    else
    {
        fadeOut();
    }
}

int MVAmpEnvelope::totalLength(int i)
{
    int n = 0;
    for(int j=0;j<NB_ENV_PTS;j++)
        n += envAmpData[i].points[j].frame;
    return n;
}

int MVAmpEnvelope::sustainLength(int i)
{
    int n = 0;
    for(int j=0;j<=envAmpData[i].sustainPoint;j++)
        n += envAmpData[i].points[j].frame;
    return n;
}

int MVAmpEnvelope::releaseLength(int i)
{
    int n = 0;
    if(envAmpData[i].sustainPoint > -1)
        for(int j=envAmpData[i].sustainPoint+1;j<NB_ENV_PTS;j++)
            n += envAmpData[i].points[j].frame;
    return n;
}

////////////////////////////////////////////////////////////////
MVAmpEnvelope::EnvelopeData MVAmpEnvelope::envAmpData[NB_OSC] =
  {  (MVAmpEnvelope::Point){96,0},(MVAmpEnvelope::Point){4000,1},(MVAmpEnvelope::Point){3000,1},(MVAmpEnvelope::Point){2000,1},(MVAmpEnvelope::Point){1000,0.5},(MVAmpEnvelope::Point){1000,0.5}, (int)3 ,(bool)false,
     (MVAmpEnvelope::Point){96,0},(MVAmpEnvelope::Point){4000,1},(MVAmpEnvelope::Point){3000,1},(MVAmpEnvelope::Point){2000,1},(MVAmpEnvelope::Point){1000,0.5},(MVAmpEnvelope::Point){1000,0.5}, (int)3 ,(bool)false,
     (MVAmpEnvelope::Point){96,0},(MVAmpEnvelope::Point){4000,1},(MVAmpEnvelope::Point){3000,1},(MVAmpEnvelope::Point){2000,1},(MVAmpEnvelope::Point){1000,0.5},(MVAmpEnvelope::Point){1000,0.5}, (int)3 ,(bool)false,
     (MVAmpEnvelope::Point){96,0},(MVAmpEnvelope::Point){4000,1},(MVAmpEnvelope::Point){3000,1},(MVAmpEnvelope::Point){2000,1},(MVAmpEnvelope::Point){1000,0.5},(MVAmpEnvelope::Point){1000,0.5}, (int)3 ,(bool)false,
     (MVAmpEnvelope::Point){96,0},(MVAmpEnvelope::Point){4000,1},(MVAmpEnvelope::Point){3000,1},(MVAmpEnvelope::Point){2000,1},(MVAmpEnvelope::Point){1000,0.5},(MVAmpEnvelope::Point){1000,0.5}, (int)3 ,(bool)false,
     (MVAmpEnvelope::Point){96,0},(MVAmpEnvelope::Point){4000,1},(MVAmpEnvelope::Point){3000,1},(MVAmpEnvelope::Point){2000,1},(MVAmpEnvelope::Point){1000,0.5},(MVAmpEnvelope::Point){1000,0.5}, (int)3 ,(bool)false
  };
