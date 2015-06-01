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

#include "MVNote.h"
#include "MVGlobals.h"
#include "MVOscillator.h"

#include <math.h>


MVNote::MVNote(const float f) : freq(f)
{
    bufferSize = MVPlayer::getBufferSize();
    bufferL = new float[bufferSize];
    bufferR = new float[bufferSize];
    for(unsigned int i=0;i<bufferSize;i++)
        bufferL[i] = bufferR[i] = 0.0;
    iBuf = 0;
    bPlaying = false;
    bReleasing = false;
    bSustainOn = false;
    nbFadeOutSteps = 0;
    amp = 0.0;

    dt = 0;
    t = 0.0;

    for(int i=0;i<NB_OSC;i++)
        oscillators[i] = new MVOscillator(i, this, t);

    createAlgo();

    pThread = 0;
    if ( ! startThread(&pThread, _noteThread, this, true, 80, 1, false))
        std::cout << ("Failed to start jack note thread");
}

MVNote::~MVNote()
{
    for(int i=0;i<NB_OSC;i++)
        delete oscillators[i];
    delete[] bufferL;
    delete[] bufferR;
}

void MVNote::setFreq(float f)
{
    freq = f;
}

void MVNote::fillBuffer()
{
    memset(bufferL,0,bufferSize * sizeof(float));
    memset(bufferR,0,bufferSize * sizeof(float));

    for(int i=0;i<NB_OSC;i++)
    {
        panR[i] = oscData[i].pan+0.5;
        panL[i] = 1.0-panR[i];
    }

    bool bFinished = false;
    iBuf = 0;

    while(iBuf < bufferSize && ! bFinished)
    {
        t = (float)dt/MVPlayer::getSampleRate();
        bFinished = ! algo();
        for(int i=0;i<NB_OSC;i++)
            oscillators[i]->next();
        dt++;
        iBuf++;
    }

    if(bFinished)
        stopPlay();
}

void MVNote::startPlay(unsigned char vel)
{
    bReleasing = false;
    bSustainOn = false;

    for(int i=0;i<NB_OSC;i++)
        oscillators[i]->reset( ! bPlaying);
    if( !  bPlaying)
    {
        dt = 0;
        t = 0.0;
    }
    amp = (float)vel/127.0f;
    bPlaying = true;
}

void MVNote::startRelease()
{
    for(int i=0;i<NB_OSC;i++)
        oscillators[i]->startRelease();
    bReleasing = true;
}
void MVNote::stopPlay()
{
    if( bReleasing)
    {
        bPlaying = false;
        bSustainOn = false;
        bReleasing = false;
        Globals::player->removePlayingNote(this);
    }
    else
    {
        startRelease();
    }
}
bool MVNote::algo()
{
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

int MVNote::getUserAlgoIndex(QString descriptor)
{
    return userAlgoDescriptor.indexOf(descriptor);
}

int MVNote::addUserAlgo(QString descriptor)
{
    userAlgoDescriptor.append(descriptor);
    return userAlgoDescriptor.count()-1;
}

void MVNote::createAlgo()
{
    if (algoType == PREDEF && preDefAlgoIndex < NB_ALGO)
        createAlgo(algoDescriptor[preDefAlgoIndex]);
    else
        if(userAlgoIndex < userAlgoDescriptor.count())
            createAlgo(userAlgoDescriptor.at(userAlgoIndex));
        else
            if(userAlgoDescriptor.count()>0)
                createAlgo(userAlgoDescriptor.at(0));
            else
                createAlgo(QString("1"));
}
void MVNote::createAlgo(int ind)
{
   if(ind < userAlgoDescriptor.count())
        createAlgo(userAlgoDescriptor.at(ind));
}

void MVNote::createAlgo(QString descriptor)
{
    for(int i=0;i<NB_OSC;i++)
        modulatorsData[i].clear();
    QStringList list = descriptor.split(' ');

    QString str = list.at(0);
    nbOps = str.length(); if(nbOps > 6) std::cout << str.toStdString() << " "<< descriptor.toStdString() << std::endl;
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
    if (sem_init(&sem, 0, 0) < 0 /*|| sem_init(&semFinished, 0, 1) < 0*/)
    {
        std::cout << "Error on jack note sem_init " << std::endl;
        return NULL;
    }

    while (true)
    {
        if (sem_wait(&sem) < 0)
        {
            std::cout << "noteThread semaphore wait error" << std::endl;
            continue;
        }
        fillBuffer();
    }
    sem_destroy(&sem);
    //sem_destroy(&semFinished);
    return NULL;
}

// Static data
MVNote::OscillatorData MVNote::oscData[NB_OSC]=
    {{0.0,0.3,1.0,1.0,5.0,0.0,5.0,0.0,5.0},
     {0.0,0.3,1.0,1.0,5.0,0.0,5.0,0.0,5.0},
     {0.0,0.3,1.0,1.0,5.0,0.0,5.0,0.0,5.0},
     {0.0,0.3,1.0,1.0,5.0,0.0,5.0,0.0,5.0},
     {0.0,0.3,1.0,1.0,5.0,0.0,5.0,0.0,5.0},
     {0.0,0.3,1.0,1.0,5.0,0.0,5.0,0.0,5.0}};

float MVNote::pitchBend = 1.0;

const char* MVNote::algoDescriptor[NB_ALGO] =
    {"13 1*2 3*4 4*5 5*6 6*6", "13 1*2 2*2 3*4 4*5 5*6", "14 1*2 2*3 4*5 5*6 6*6", "14 1*2 2*3 4*5 5*6 6*4", "135 1*2 3*4 5*6 6*6", "135 1*2 3*4 5*6 6*5", "13 1*2 3*4 3*5 5*6 6*6", "13 1*2 3*4 3*5 4*4 5*6",
     "13 1*2 2*2 3*4 3*5 5*6", "14 1*2 2*3 3*3 4*5 4*6", "14 1*2 2*3 4*5 4*6 6*6", "13 1*2 2*2 3*4 3*5 3*6", "13 1*2 3*4 3*5 3*6 6*6", "13 1*2 3*4 4*5 4*6 6*6", "13 1*2 2*2 3*4 4*5 4*6", "1 1*2 1*3 1*5 3*4 5*6 6*6",
     "1 1*2 2*2 1*3 1*5 3*4 5*6", "1 1*2 1*3 3*3 1*4 4*5 5*6", "145 1*2 2*3 4*6 5*6 6*6", "124 1*3 2*3 3*3 4*5 4*6", "1245 1*3 2*3 3*3 4*6 5*6", "1345 1*2 3*6 4*6 5*6 6*6", "1245 2*3 4*6 5*6 6*6", "12345 3*6 4*6 5*6 6*6",
     "12345 4*6 5*6 6*6", "124 2*3 4*5 4*6 6*6", "124 2*3 3*3 4*5 4*6", "136 1*2 3*4 4*5 5*5", "1235 3*4 5*6 6*6", "1236 3*4 4*5 5*5", "12345 5*6 6*6", "123456 6*6"
    };

QStringList MVNote::userAlgoDescriptor;
MVNote::AlgoType MVNote::algoType = PREDEF;
int MVNote::preDefAlgoIndex = 0;
int MVNote::userAlgoIndex = 0;
int MVNote::nbOps = 0;
int MVNote::ops[NB_OSC];
QList<int> MVNote::modulatorsData[NB_OSC];
