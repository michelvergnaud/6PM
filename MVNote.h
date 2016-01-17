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

class MVAmpEnvelope;
class MVFreqEnvelope;
class MVOscillator;

class MVNote
{
public:
    MVNote(const float f);
    void setFreq(float);
    ~MVNote();

    struct OscillatorData
    {
        float pan;
        float amp;
        float freqratio;
        float detune;
        float iMod;
        float tremAmp;
        float tremFreq;
        int tremShape;
        float vibAmp;
        float vibFreq;
        int vibShape;
        float instantFreq;
        float randomize;
    };

    enum AlgoType {PREDEF, USER};

    void startPlay(unsigned char);
    void startRelease();
    void stopPlay();
    inline void stopNow(){bPlaying = false;sem_post(&sem);/*pThread=0;*/}
    inline const float & getFreq(){return freq;}
    inline const float & getAmp(){return amp;}
    inline void sustain(){ bSustainOn = true;}
    inline bool isSustained(){return bSustainOn;}
    inline bool isPlaying(){return bPlaying;}
    inline const MVOscillator & getOscillator(int i){return *oscillators[i];}

    void fillBuffer();
    void skipBuffer();

    inline void addToMainBuffer(unsigned int i, float &left, float&right)
    {
        left += bufferL[i];
        right += bufferR[i];
        if(i+1 == bufferSize)
            sem_post(&sem);
    }

    static float pitchBend;

    static OscillatorData oscData[NB_OSC];    
    static const char* algoDescriptor[NB_ALGO];
    static QStringList userAlgoDescriptor;
    static int getUserAlgoIndex(QString);
    static int addUserAlgo(QString);

    static int preDefAlgoIndex;
    static int userAlgoIndex;

    static QList<int> modulatorsData[NB_OSC];
    static int ops[NB_OSC]; // final oscillators (operators)
    static int nbOps;
    static bool isOp(int);
    static bool isModulated(int);
    static bool isModulator(int);
    static void createAlgo();
    static void createAlgo(int);
    static void createAlgo(QString);
    static AlgoType algoType;

private:

    int dt;
    float t;
    float freq;
    float panL[NB_OSC];
    float panR[NB_OSC];
    float v[NB_OSC];
    float amp;

    MVOscillator * oscillators[NB_OSC];

    bool bSustainOn;
    bool bPlaying;
    bool bReleasing;

    float * bufferL;
    float * bufferR;
    unsigned int bufferSize;
    unsigned int iBuf;

    bool algo();
    float fadeOutCoeff;
    int nbFadeOutSteps;

    pthread_t pThread;
    sem_t sem;


    static void *_noteThread(void *arg);
    void * noteThread(void);
};

#endif // MVNOTE_H
