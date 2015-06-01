/*
    ZynAddSubFx effects controller
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

#ifndef CONVERTER_H
#define CONVERTER_H

#include <QMap>
#include "globals.h"
#include "RtMidi.h"
#include <jack/midiport.h>

class MVController;

class MVConverter
{
public:
    MVConverter();
    void convert(jack_nframes_t nframes /*double deltatime, std::vector< unsigned char > *message, void *userData*/);

    void setMidiLearning(bool b, MVController * c);

    void sendMidiMessage(std::vector< unsigned char > * message);
    bool isMidiLearning();
    MVController * controllerLearning();

    MVController * getController(int);
    int getAssignNum(MVController*);
    void registerAssign(int, MVController*);


private:

    QMap<int, MVController *> assignMap;

    RtMidiIn *midiin;
    jack_client_t *jackMidiClient;
    jack_port_t *jackMidiPort;


    unsigned char runningStatus;
    bool bMidiLearning;
    MVController * ctrLearning;
};

#endif // CONVERTER_H
