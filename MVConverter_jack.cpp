/*
    MVSynth effects controller
     Copyright (c) 2013, Michel Vergnaud

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

#include "globals.h"
#include "MVConverter.h"
#include "MVPlayer.h"
#include "RtMidi.h"
#include "MVController.h"
#include <math.h>

int jackMidiCallback(jack_nframes_t nframes, void*)
{
    Globals::converter->convert(nframes);
    return 0;
}

MVConverter::MVConverter()
{
    //Globals::converter = this;
    bMidiLearning = false;
    ctrLearning = NULL;
    midiin=0;
    runningStatus = 0;

    // ---------Jack
    if ((jackMidiClient = jack_client_open("MVAddSynth", JackNullOption, NULL)) == 0)
    {
        std::cout << "(open) JACK server not found, aborting...";
        exit(false);
    }
    jackMidiPort = jack_port_register(jackMidiClient, "midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    if(jackMidiPort == NULL)
    {
        std::cout << "Midi input port could not be registered, aborting...";
        exit(false);
    }

      jack_set_process_callback(jackMidiClient, jackMidiCallback, NULL);

      if (jack_activate(jackMidiClient) !=0)
      {
        std::cout  <<  "(open) jackMidiClient: Can't activate JACK.";
        exit(false);
      }

}

void MVConverter::convert(jack_nframes_t nframes )
{
   void* buffer = jack_port_get_buffer(jackMidiPort, nframes);
   if(!buffer) return;
   const int event_count = jack_midi_get_event_count(buffer);
   jack_midi_event_t ev;

   for (int i = 0; i < event_count; ++i)
   {
        jack_midi_event_get(&ev, buffer, i);
        if (ev.buffer == NULL)
        {
            return;             
        }
   }

    char * message = (char*)(ev.buffer);
    int nBytes = ev.size;
    unsigned char status = 0;
    unsigned char note = 0;
    unsigned char velo = 0;

    if(nBytes == 3)
    {
        status = message[0];
        runningStatus = status;
    }
    else
    if(nBytes == 2)
    {
        status = runningStatus;
    }
    else
    {
        runningStatus = 0;
        return;
    }
    // Is it a note message ?
    if(status >= 0x90 && status <= 0x9F) // note on
    {
        if( nBytes==3)
        {
            note = message[1];
            velo = message[2];
        }
        else if( nBytes==2)
        {
            note = message[0];
            velo = message[1];
        }
        if(note>20 && note <= 127)
        {
            if(velo > 0)
            {
                Globals::player->startPlay(note, velo);
            }
            else
            {
                Globals::player->stopPlay(note);
            }
        }
    }
    else
    if(status >= 0x80 && status <= 0x8F) // note off
    {
        if( nBytes==3)
            note = message[1];
        else if( nBytes==2)
                note = message[0];
        if(note>20 && note <= 127)
            Globals::player->stopPlay(note);
    }
    else
    // Is it a control message ?
    if(nBytes == 3 && message[0] == (char)0xB0)
    {
        unsigned char ccnumber = message[1];
        if(bMidiLearning && ctrLearning != NULL)
        {
           registerAssign(ccnumber, ctrLearning);
           bMidiLearning=false;
           ctrLearning->stopWaitingMessage();
           ctrLearning = NULL;
           return;
        }
        // Search for controller managing this control number
        MVController * controller = getController(ccnumber);
        if (controller != NULL)
        {
            unsigned char ccvalue = message[2];
            controller->setParamValue(ccvalue);
        }
        else
        {
            if( message[1] == (unsigned char)0x40) // Sustain
                Globals::player->setSustainOn(message[2]>63);
        }

    }

}

void MVConverter::registerAssign(int ctrlNum, MVController* controller)
{
    // Deletele old assignment of this control number
    MVController * oldAssigned = assignMap.take(ctrlNum);
    if(oldAssigned != NULL)
          oldAssigned->setAssignNumber(0);

    if(controller != NULL)
    {
        // Remove controller assignment
        int oldKey = assignMap.key(controller);
        if(oldKey>0)
            assignMap.remove(oldKey);
        // Set new entry
        if (ctrlNum >0)
            assignMap.insert(ctrlNum, controller);
        else
            assignMap.remove(assignMap.key(controller));
        controller->setAssignNumber(ctrlNum);
    }
    else
    {
        assignMap.remove(ctrlNum);
    }
}

MVController * MVConverter::getController(int ctrlNum)
{
       return assignMap.value(ctrlNum);
}
int MVConverter::getAssignNum(MVController * controller)
{
    return assignMap.key(controller);
}

 void MVConverter::setMidiLearning(bool b, MVController * c)
 {


     if(b || (c == ctrLearning)) // only sender can stop
     {
        bMidiLearning = b;
        ctrLearning = c;
     }

 }

bool MVConverter::isMidiLearning()
{
    return bMidiLearning;
}

MVController * MVConverter::controllerLearning()
{
    return ctrLearning;
}


