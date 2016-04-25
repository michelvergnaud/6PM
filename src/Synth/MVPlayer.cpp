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

#include "MVPlayer.h"
#include "MVControlManager.h"
#include "UI/MVMainFrame.h"
#include "MVNote.h"
#include "MVOscillator.h"
#include "MVGlobals.h"

#include <math.h>
#include <iostream>
#include <jack/jack.h>
#include <jack/thread.h>

#include <QSettings>

typedef jack_default_audio_sample_t sample_t;
jack_nframes_t MVPlayer::sampleRate = 96000;
unsigned int MVPlayer::bufferSize = 256;
float MVPlayer::mainVolume = 0.5;

unsigned int MVPlayer::maxFreq = 48000;

jack_client_t * client;
jack_native_thread_t threadConstructBuffer;


int process_callback(jack_nframes_t nframes, void *)
{
    Globals::player->constructBuffer();
    Globals::player->processMidi(nframes);

    return 0;
}

int xrun_callback(void *)
{
    std::cout << "xrun" << std::endl;
    return 0;
}

MVPlayer::MVPlayer()
{    
    Globals::player = this;

    midi.port = NULL;
    midi.ringBuf = NULL;
    midi.pThread = 0;
//    bSkipBuffer = false;
//    offset=0;
    jack_status_t status;
    client = jack_client_open ("6PM", JackNullOption, &status);
    if(client == NULL)
        std::cout << "Cannot create client" << std::endl;

    output_port_left = jack_port_register (client, "audio_output_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    output_port_right = jack_port_register (client, "audio_output_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    sampleRate = jack_get_sample_rate(client);
    bufferSize =  jack_get_buffer_size (client);
    maxFreq = sampleRate/2;

    maxBufL = 0.0;
    maxBufR = 0.0;

    MVFreqEnvelope::timeValues = new float[MVFreqEnvelope::nbTimeValues];

    for(int i=0;i<10;i++)
        MVFreqEnvelope::timeValues[i] = (i+1);
    for(int i=10;i<28;i++)
        MVFreqEnvelope::timeValues[i] = (i-10+1)*5 + 10;
    for(int i=28;i<46;i++)
        MVFreqEnvelope::timeValues[i] = (i-28+1)*50 + 100;
    for(int i=46;i<=63;i++)
        MVFreqEnvelope::timeValues[i] = (i-46+1)*500 + 1000;


    QSettings settings;
    diapason = settings.value("diapason",440).toInt();
    pitchBendRange = settings.value("pitchBendRange",2).toInt();
    for(int i=0;i<NB_NOTES;i++)
    {
      float f = diapason * pow(2.0f, (float)(FIRST_NOTE + i - 69) / 12.0f);
      notes[i] = new MVNote(f);
    }
    bSustainOn = false;
    mainVolume = (float)settings.value("mainVolume", 50 ).toInt()/100.0;
    midiChannel = settings.value("midiChannel",0).toInt();
    maxPolyphony = settings.value("maxPolyphony",16).toInt();

    playingNotes = new MVNote*[NB_NOTES+1];
    for(int i=0;i<NB_NOTES+1;i++)
        playingNotes[i] = NULL;
    posNextNote = 0;

    startJack() ;
}

MVPlayer::~MVPlayer()
{
    jack_client_close (client);
    for(int i=0;i<NB_NOTES;i++)
        delete notes[i];
    delete[] playingNotes;
}

void MVPlayer::startJack()
{
    if ( ! openMidi())
        std::cout << "Couldn't open midi" << std::endl;

    if (midi.port && ! startThread(&midi.pThread, _midiThread, this, true, 50, 1, false))
    {
        std::cout << ("Failed to start jack midi thread");
    }

    jack_set_process_callback( client, process_callback, 0);
    jack_set_xrun_callback(client, xrun_callback,0);

    if (jack_activate (client) != 0)
    {
        std::cout << "cannot activate client";
    }
    if ((ports = jack_get_ports (client, NULL, NULL, JackPortIsPhysical|JackPortIsInput)) == NULL)
    {
        std::cout << "Cannot find any physical playback ports\n";
    }

    int i=0;
    int nbCnx = 0;
    while(ports[i]!=NULL && nbCnx<1)
    {
        if (jack_connect (client, jack_port_name (output_port_left), ports[i]) != 0)
        {
          std::cout <<  "cannot connect output ports\n";
        }
        else
            nbCnx++;
        i++;
    }
    nbCnx = 0;
    while(ports[i]!=NULL && nbCnx<1)
    {
        if (jack_connect (client, jack_port_name (output_port_right), ports[i]) != 0)
        {
          std::cout <<  "cannot connect output ports\n";
        }
        else
            nbCnx++;
        i++;
    }

    free (ports);
}

void MVPlayer::polyphonyChanged(int n)
{
    maxPolyphony = n;
    posNextNote = qMin(posNextNote, maxPolyphony);
}

void MVPlayer::removePlayingNote(MVNote * n)
{
    int i=0;

    while (i<maxPolyphony)
    {
        if (playingNotes[i] == n)
        {
            if(i == maxPolyphony-1)
            {
                playingNotes[i] = NULL;
                posNextNote = i;
            }
            else
            {
                memmove(playingNotes+i, playingNotes+i+1,(maxPolyphony-i)*sizeof(MVNote*));
                posNextNote--;
            }
        }
        else
            i++;
    }
}

void MVPlayer::startPlay(unsigned char note, unsigned char velo)
{
    int ind = note-FIRST_NOTE;
    if(ind >= 0 && ind < NB_NOTES)
    {
        MVNote * newNote = notes[ind];
        if(newNote->isPlaying())
            removePlayingNote(newNote);

        if(posNextNote == maxPolyphony)
        {
           // std::cout << "Polyphony overflow" << std::endl;
            playingNotes[0]->stopNow();
            memmove(playingNotes, playingNotes+1, (maxPolyphony-1)*sizeof(MVNote*));
            playingNotes[maxPolyphony-1] = newNote;
        }
        else
        {
            playingNotes[posNextNote] = newNote;
            posNextNote++;
        }
        newNote->startPlay(velo);
    }
    else
        std::cout << "Note out of range" << std::endl;
}

void MVPlayer::stopPlay(unsigned char note)
{
    int ind = note-FIRST_NOTE;
    if(ind >= 0 && ind < NB_NOTES)
    {

        if(bSustainOn)
            notes[ind]->sustain();
        else
            notes[ind]->stopPlay();
    }
}

void MVPlayer::stopAllNotes()
{
    for(int i=0;i<NB_NOTES;i++)
        notes[i]->stopNow();
    for(int i=0;i<maxPolyphony;i++)
        playingNotes[i] = NULL;
    bSustainOn = false;
}

//void MVPlayer::skipBuffer()
//{
//    bSkipBuffer = true;
//}

void MVPlayer::setSustainOn(bool b)
{
    bSustainOn = b;
    if(! b)
    {
        for(int i=0;i<maxPolyphony;i++)
            if(playingNotes[i] != NULL && playingNotes[i]->isSustained())
                playingNotes[i]->stopPlay();
    }
}

void MVPlayer::midiChannelChanged(int n)
{
    if(n>=0 && n<16)
        midiChannel = n-1;
}

void MVPlayer::setDiapason(float n)
{
    diapason = n;
    for(int i=0;i<NB_NOTES;i++)
    {
        float f = diapason * pow(2.0f, (float)(FIRST_NOTE + i - 69) / 12.0f);
        notes[i]->setFreq(f);
    }
}

bool MVPlayer::openMidi(void)
{
    const char *port_name = "midi in";
    midi.port = jack_port_register(client, port_name,
                                   JACK_DEFAULT_MIDI_TYPE,
                                   JackPortIsInput, 0);
    if (!midi.port)
    {
        std::cout << "Impossible to register midi port" << std::endl;
        return false;
    }
    midi.ringBuf = jack_ringbuffer_create(sizeof(struct midi_event) * 4096);
    if (!midi.ringBuf)
    {
        std::cout << "Impossible to create midi ring buffer" << std::endl;
        return false;
    }
    if (jack_ringbuffer_mlock(midi.ringBuf))
    {
        std::cout << "Impossible to lock midi port" << std::endl;
        return false;
    }
    return true;
}

void *MVPlayer::_midiThread(void *arg)
{
    return static_cast<MVPlayer*>(arg)->midiThread();
}

void *MVPlayer::midiThread(void)
{
    unsigned char channel, note, velocity;
    int ctrlNum;
    int par = 0;
    unsigned int fetch;
    unsigned int ev;
    struct midi_event midiEvent;
    if (sem_init(&midiSem, 0, 0) < 0)
    {
        std::cout << "Error on jack midi sem_init " << std::endl;
        return NULL;
    }

    while (true)
    {
        if (sem_wait(&midiSem) < 0)
        {
            std::cout << "midiThread semaphore wait error" << std::endl;
            continue;
        }
        fetch = jack_ringbuffer_read(midi.ringBuf, (char*)&midiEvent, sizeof(struct midi_event));
        if (fetch != sizeof(struct midi_event))
        {
            std::cout << "Short ringbuffer read, " << std::endl;
            continue;
        }
        channel = midiEvent.data[0] & 0x0F;

        if (midiChannel == -1 || channel == midiChannel)
        {
            switch ((ev = midiEvent.data[0] & 0xF0))
            {
                case 0x07: // Volume
                    par = midiEvent.data[2];
                    Globals::controlmanager->processCC(0x07, par);
                    break;
                case 0x7B:  // all notes off
                    stopAllNotes();
                    break;
                case 0x80: // note-off
                    note = midiEvent.data[1];
                    stopPlay(note);
                    break;
                case 0x90: // note-on
                    if ((note = midiEvent.data[1])) // skip note == 0
                    {
                        velocity = midiEvent.data[2];
                        startPlay(note, velocity);
                    }
                    break;
                case 0xB0: // controller
                    ctrlNum = midiEvent.data[1];
                    par = midiEvent.data[2];
                    if(ctrlNum == 0x00) // Bank select
                    {
                        if(Globals::frame != NULL)
                            Globals::frame->bankSelect(par);
                        else
                            Globals::presetManager->setCurrentBank(par);
                    }
                    else
                    if (ctrlNum == 0x78) // All sound off
                        stopAllNotes();
                    else
                    if (ctrlNum == 0x40) // Sustain
                        Globals::player->setSustainOn(par>63);
                    else
                        Globals::controlmanager->processCC(ctrlNum, par);
                    break;

                case 0xC0: // program change
                    par = midiEvent.data[1];
                    if(Globals::frame != NULL)
                        Globals::frame->loadPreset(par);
                    else
                        Globals::presetManager->setCurrentPreset(par);
                    break;

                case 0xE0: // pitch bend
                    par = ((midiEvent.data[2] << 7) | midiEvent.data[1]) - 8192 ;
                    MVNote::pitchBend = powf(TWELFTH_ROOT2, pitchBendRange * (float)par / 8192 );
                    break;
                default:
                    break;
            }
        }
    }
    sem_destroy(&midiSem);
    return NULL;
}

bool MVPlayer::processMidi(jack_nframes_t nframes)
{
    void *portBuf = jack_port_get_buffer(midi.port, nframes);
    if (!portBuf)
    {
        std::cout << "Bad midi jack_port_get_buffer" << std::endl;
        return  false;
    }

    unsigned int idx;
    unsigned int act_write;
    struct midi_event event;
    char *data;
    unsigned int wrote = 0;
    unsigned int tries = 0;
    jack_midi_event_t jEvent;
    jack_nframes_t eventCount = jack_midi_get_event_count(portBuf);
    for(idx = 0; idx < eventCount; ++idx)
    {
        if(!jack_midi_event_get(&jEvent, portBuf, idx))
        {
            if ( (jEvent.size < 1) || jEvent.size > sizeof(event.data) )
                continue; // no interest in zero sized or long events
            event.time = jEvent.time;
            memset(event.data, 0, sizeof(event.data));
            memcpy(event.data, jEvent.buffer, jEvent.size);
            wrote = 0;
            tries = 0;
            data = (char*)&event;
            while (wrote < sizeof(struct midi_event) && tries < 3)
            {
                act_write = jack_ringbuffer_write(midi.ringBuf, (const char*)data,
                                                  sizeof(struct midi_event) - wrote);
                wrote += act_write;
                data += act_write;
                ++tries;
            }
            if (wrote == sizeof(struct midi_event))
            {
                if (sem_post(&midiSem) < 0)
                    std::cout << "processMidi semaphore post error" << std::endl;
            }
            else
            {
                std::cout << "Bad write to midi ringbuffer" << std::endl;
                return false;
            }
        }
        else
            std::cout << "... jack midi read failed" << std::endl;
    }
    return true;
}
