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
    void setSustainOn(bool);
    void setDiapason(float);

    int getMidiChannel(){return midiChannel;}
    int getDiapason(){return diapason;}
    int getPolyphony() { return maxPolyphony;}
    int getPitchBendRange(){return pitchBendRange;}

    inline void constructBuffer()
    {
        memset(buffer_L,0,bufferSize*sizeof(float));
        memset(buffer_R,0,bufferSize*sizeof(float));

        for(unsigned int i=0; i<bufferSize;i++)
        {
//            for(int j = 0; j<NB_NOTES;j++)
//            {
//                MVNote * note = notes[j];
//                if( note != NULL && note->isPlaying())
//                    note->addToMainBuffer(i, buffer_L[i], buffer_R[i] );
//            }

            for(int j = 0; j<posNextNote;j++)
            {
                MVNote * note = playingNotes[j];

                if( note != NULL && note->isPlaying())
                {
                    //sem_wait(&note->semFinished);
                    note->addToMainBuffer(i, buffer_L[i], buffer_R[i] );
                }
            }
            buffer_L[i]*=mainVolume;
            buffer_R[i]*=mainVolume;
        }
    }

    inline void copyBuffers()
    {
        memcpy(out_L, buffer_L,bufferSize*sizeof(float));
        memcpy(out_R, buffer_R,bufferSize*sizeof(float));
    }

    bool processMidi(jack_nframes_t nframes);
    void removePlayingNote(MVNote *);
    int posNextNote;

    static inline unsigned int getSampleRate() {return sampleRate;}
    static unsigned int getBufferSize() {return bufferSize;}
    static float mainVolume;

    inline float * getBufferL() const {return out_L;}
    inline float * getBufferR() const {return out_R;}

public slots :
    void midiChannelChanged(int);
    void polyphonyChanged(int);
    void pitchBendRangeChanged(int n){pitchBendRange = n;}

private:
    const char **ports;
    jack_port_t *output_port_left;
    jack_port_t *output_port_right;
    typedef jack_default_audio_sample_t sample_t;
    sample_t* cycle;
    static jack_nframes_t sampleRate;
    static jack_nframes_t bufferSize;
    long offset;
    int rtprio;
    struct {
        jack_port_t*       port;
        jack_ringbuffer_t *ringBuf;
        pthread_t          pThread;
    } midi;

    sem_t midiSem;

    struct midi_event {
        jack_nframes_t time;
        char data[4]; // all events of interest are <= 4bytes
    };

    bool openMidi(void);
    static void *_midiThread(void *arg);
    void *midiThread(void);

    sample_t *buffer_L;
    sample_t *buffer_R;

    sample_t *out_L;
    sample_t *out_R;
    float vL, vR;
    float diapason;
    int pitchBendRange;
    MVNote * notes[127];
    MVNote ** playingNotes;
    int maxPolyphony;
    bool bSustainOn;
    int midiChannel;
};

#endif // MV_WAVEPLAYER_H
