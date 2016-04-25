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

    Some parts of this code are part of "yoshimi" code
    http://sourceforge.net/projects/yoshimi/
    Copyright 2009-2011, Alan Calvert
    Copyright 2014, Will Godfrey
*/

#ifndef MV_WAVEPLAYER_H
#define MV_WAVEPLAYER_H

#include "MVDefines.h"
#include "MVUtils.h"
#include "MVNote.h"

#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>
#include <math.h>

#include <QObject>

class MVMeter;

class MVPlayer : public QObject
{
    Q_OBJECT

public:

    MVPlayer();
     ~MVPlayer();

    void startJack();
    void startPlay(unsigned char note, unsigned char velo);
    void stopPlay(unsigned char note);

    void stopAllNotes();
    //void skipBuffer();
    void setSustainOn(bool);
    void setDiapason(float);

    int getMidiChannel(){return midiChannel;}
    int getDiapason(){return diapason;}
    int getPolyphony() { return maxPolyphony;}
    int getPitchBendRange(){return pitchBendRange;}

    inline void constructBuffer()
    {
//        bSkipBuffer = false;
        sample_t * buffer_L = (sample_t*)jack_port_get_buffer(output_port_left, bufferSize);
        sample_t * buffer_R = (sample_t*)jack_port_get_buffer(output_port_right, bufferSize);

//        maxBufL = 0.0;
//        maxBufR = 0.0;

        for(unsigned int i=0; i<bufferSize;i++)
        {
            buffer_L[i] = 0;
            buffer_R[i] = 0;
            for(int j=0; j<posNextNote;j++)
            {
                MVNote * note = playingNotes[j];
                if( note != NULL && note->isPlaying())
                    note->addToMainBuffer(i, buffer_L[i], buffer_R[i] );
            }
            buffer_L[i]*=mainVolume;
            buffer_R[i]*=mainVolume;

            maxBufL = qMax(maxBufL,buffer_L[i]);
            maxBufR = qMax(maxBufR,buffer_R[i]);
        }
    }

    bool processMidi(jack_nframes_t nframes);
    void removePlayingNote(MVNote *);
    int posNextNote;

    inline float & getMaxBufL(){return maxBufL;}
    inline float & getMaxBufR(){return maxBufR;}

    static inline unsigned int getSampleRate() {return sampleRate;}
    static unsigned int getBufferSize() {return bufferSize;}
    static unsigned int getMaxFreq(){return maxFreq;}
    static float mainVolume;

public slots :
    void midiChannelChanged(int);
    void polyphonyChanged(int);
    void pitchBendRangeChanged(int n){pitchBendRange = n;}

private:
    const char **ports;
    jack_port_t *output_port_left;
    jack_port_t *output_port_right;
    typedef jack_default_audio_sample_t sample_t;

    static jack_nframes_t sampleRate;
    static jack_nframes_t bufferSize;
    static unsigned int maxFreq;
//    long offset;

    struct {
        jack_port_t*       port;
        jack_ringbuffer_t *ringBuf;
        pthread_t          pThread;
    } midi;

    sem_t midiSem;
 //   bool bSkipBuffer;
    struct midi_event {
        jack_nframes_t time;
        char data[4];
    };

    bool openMidi(void);
    static void *_midiThread(void *arg);
    void *midiThread(void);

    float maxBufL, maxBufR;
    float diapason;
    int pitchBendRange;
    MVNote * notes[127];
    MVNote ** playingNotes;
    int maxPolyphony;
    bool bSustainOn;
    int midiChannel;
};

#endif // MV_WAVEPLAYER_H
