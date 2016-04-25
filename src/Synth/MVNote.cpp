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

#include "MVNote.h"
#include "MVGlobals.h"
#include "MVOscillator.h"
#include "MVFilter.h"

#include <math.h>

MVNote::MVNote(const float f)
{
    freq = f;
    bufferSize = MVPlayer::getBufferSize();
    bufferL = new float[bufferSize];
    bufferR = new float[bufferSize];
    for(unsigned int i=0;i<bufferSize;i++)
        bufferL[i] = bufferR[i] = 0.0;
    iBuf = 0;
    bPlaying = false;
    bReleasing = false;
    bLastBuf = false;
    bSustainOn = false;

    amp = 0.0f;

    filterL = new MVFilter(MVPlayer::getSampleRate(), freq, *Globals::presetManager->getCurrentPreset()->getFilterData());
    filterR = new MVFilter(MVPlayer::getSampleRate(), freq, *Globals::presetManager->getCurrentPreset()->getFilterData());

    dt = 0;
    t = 0.0f;

    for(int i=0;i<NB_OSC;i++)
        oscillators[i] = new MVOscillator(i, this, t);

    createAlgo();

    pThread = 0;
}

MVNote::~MVNote()
{
    for(int i=0;i<NB_OSC;i++)
        delete oscillators[i];
    delete[] bufferL;
    delete[] bufferR;
    delete filterL;
    delete filterR;
}


void MVNote::setFreq(float f)
{
    freq = f;
}

void MVNote::fillBuffer()
{
    bFilling = true;
    initBuffer();

    for(int i=0;i<NB_OSC;i++)
    {
        panR[i] = Globals::presetManager->getCurrentPreset()->getOscData(i)->pan+0.5f;
        panL[i] = 1.0f-panR[i];
    }

    bool bFinished = false;
    iBuf = 0;

    while(iBuf < bufferSize && ! bFinished)
    {
        t = (float)dt/MVPlayer::getSampleRate();

        bFinished = ! computeSample();

        if(Globals::presetManager->getCurrentPreset()->getFilterData()->used)
        {
            bufferL[iBuf] = filterL->computeOut(bufferL[iBuf]);
            bufferR[iBuf] = filterR->computeOut(bufferR[iBuf]);
        }

        for(int i=0;i<NB_OSC;i++)
            oscillators[i]->next();
        dt++;
        // Avoid rounding errors with too big numbers when note last very long. Seems ok every few minutes
        if(dt>96000 * 120)dt=0;
        iBuf++;
    }
    if(bLastBuf)
    {
        bLastBuf=false;
        stopPlay();
    }else if(bFinished)
        bLastBuf=true;

    bFilling = false;
}

void MVNote::startPlay(unsigned char vel)
{
    bReleasing = false;
    bSustainOn = false;
    initBuffer();
    filterL->reset();
    filterR->reset();

    for(int i=0;i<NB_OSC;i++)
        oscillators[i]->reset( ! bPlaying);
    if( !  bPlaying)
    {
        dt = 0;
        t = 0.0f;
    }
    amp = (float)vel/127.0f;

    if( ! bPlaying)
    {
        bPlaying = true;
        if(! startThread(&pThread, _noteThread, this, true, 30, 1, true))
        {
            bPlaying = false;
            std::cout << "Failed to start note thread" << std::endl;
        }
    }
}

void MVNote::startRelease()
{
    for(int i=0;i<NB_OSC;i++)
        oscillators[i]->startRelease();
   if(Globals::presetManager->getCurrentPreset()->getFilterData()->used)
   {
       filterL->startRelease();
       filterR->startRelease();
   }

    bReleasing = true;
}

void MVNote::stopPlay()
{
    if( bReleasing)
    {
        bPlaying = false;
        sem_post(&sem);
        bSustainOn = false;
        bReleasing = false;
        Globals::player->removePlayingNote(this);
    }
    else
    {
        startRelease();
    }
}
bool MVNote::computeSample()
{
    bufferL[iBuf] = 0.0;
    bufferR[iBuf] = 0.0;

    for(int i=0;i<nbOps;i++)
    {
        v[i] = oscillators[ops[i]]->value();
    }
    for(int i=0;i<nbOps;i++)
    {
        bufferL[iBuf] += panL[ops[i]] * v[i];
        bufferR[iBuf] += panR[ops[i]] * v[i];
    }
    bool bFinished = true;
    int i = 0;
    while(i<nbOps && bFinished)
        bFinished &= oscillators[ops[i++]]->isFinished();

    return ! bFinished;
}

void MVNote::createAlgo()
{
    if (Globals::presetManager->getCurrentPreset()->isPreDefAlgo())
        createAlgo(Globals::presetManager->getPredefAlgoDescriptor());
    else
        createAlgo(Globals::presetManager->getUserAlgoDescriptor());
}

void MVNote::createAlgo(QString descriptor)
{
    for(int i=0;i<NB_OSC;i++)
        modulatorsData[i].clear();
    QStringList list = descriptor.split(' ');

    QString str = list.at(0);
    nbOps = str.length();
    for(int i=0;i<nbOps;i++)
         ops[i] = str.at(i).digitValue() - 1;

    for(int i=1;i<list.length();i++)
    {
        str = list.at(i);
        if(str.length()==3 && str.at(1) == '*')
        {
            int carrier = str.at(0).digitValue()-1;
            if(modulatorsData[carrier].count() < NB_OSC)
            {
                int modulator = str.at(2).digitValue()-1;
                modulatorsData[carrier].append(modulator);
                qSort(modulatorsData[carrier]);
            }
        }
    }
}

bool MVNote::isOp(int n)
{
    bool b = false;
    int i = 0;
    while(i<nbOps && !b)
    {
        if(n == ops[i])
            b=true;
        i++;
    }
    return b;
}
bool MVNote::isModulated(int n)
{
    return modulatorsData[n].count() >0;
}

bool MVNote::isModulator(int n)
{
    int i=0;
    bool b = false;
    while(i<NB_OSC)
        if(modulatorsData[i++].contains(n))
            b = true;
    return b;
}

void *MVNote::_noteThread(void *arg)
{
    return static_cast<MVNote*>(arg)->noteThread();
}


void *MVNote::noteThread(void)
{
    bRunning = true;
    if (sem_init(&sem, 0, 0) < 0 )
    {
        std::cout << "Error on jack note sem_init " << std::endl;
        return NULL;
    } 
    while (bPlaying)
    {
        for(int i=0;i<NB_OSC;i++)
        {
            OscillatorData * oscData = Globals::presetManager->getCurrentPreset()->getOscData(i);
            oscData->instantFreq = oscData->freqratio * oscData->detune;
            if(oscData->randomize > 0.0)
                oscData->instantFreq *= (1.0 + oscData->randomize * ((float)(rand())/RAND_MAX - 0.5));
        }

        if (sem_wait(&sem) < 0)
        {
            std::cout << "noteThread semaphore wait error" << std::endl;
            continue;
        }
        fillBuffer();
    }
    sem_destroy(&sem);

    bRunning = false;

    return NULL;
}

float MVNote::pitchBend = 1.0f;
int MVNote::nbOps = 0;
int MVNote::ops[NB_OSC];
QList<int> MVNote::modulatorsData[NB_OSC];

