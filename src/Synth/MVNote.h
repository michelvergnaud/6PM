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

#ifndef MVNOTE_H
#define MVNOTE_H

#include "MVDefines.h"
#include "MVUtils.h"
#include "MVGlobals.h"

#include <jack/jack.h>
#include <jack/thread.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <QStringList>

class MVOscillator;
class MVFilter;

class MVNote
{
public:
    MVNote(const float f);
    void setFreq(float);
    ~MVNote();

    void startPlay(unsigned char);
    void startRelease();
    void stopPlay();
    inline void stopNow(){bPlaying = false;if(bRunning)sem_post(&sem);initBuffer();}
    inline const float & getFreq(){return freq;}
    inline const float & getAmp(){return amp;}
    inline void sustain(){ bSustainOn = true;}
    inline bool isSustained(){return bSustainOn;}
    inline bool isPlaying(){return bPlaying;}
    inline bool isFillingBuffer(){return bFilling;}
    inline const MVOscillator & getOscillator(int i){return *oscillators[i];}
    void fillBuffer();

    inline void initBuffer()
    {
        memset(bufferL,0,bufferSize * sizeof(float));
        memset(bufferR,0,bufferSize * sizeof(float));
    }
    inline void addToMainBuffer(unsigned int i, float &left, float&right)
    {
        left += bufferL[i];
        right += bufferR[i];
        if(i+1 == bufferSize)
            sem_post(&sem);
    }

    static float pitchBend;
    static QList<int> modulatorsData[NB_OSC];
    static int ops[NB_OSC];
    static int nbOps;
    static bool isOp(int);
    static bool isModulated(int);
    static bool isModulator(int);
    static void createAlgo();
    static void createAlgo(QString);

private:

    int dt;
    float t;
    float freq;
    float panL[NB_OSC];
    float panR[NB_OSC];
    float v[NB_OSC];
    float amp;
    MVOscillator * oscillators[NB_OSC];
    MVFilter * filterL;
    MVFilter * filterR;
    bool bSustainOn;
    bool bPlaying;
    bool bFilling;
    bool bReleasing;
    bool bRunning;
    bool bLastBuf;
    float * bufferL;
    float * bufferR;
    unsigned int bufferSize;
    unsigned int iBuf;
    bool computeSample();

    pthread_t pThread;
    sem_t sem;
    struct timespec ts;
    static void *_noteThread(void *arg);
    void * noteThread(void);
};

#endif // MVNOTE_H
