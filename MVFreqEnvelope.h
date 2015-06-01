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

#ifndef MVFREQENVELOPE_H
#define MVFREQENVELOPE_H

#include "MVAmpEnvelope.h"
#include "MVNote.h"

class MVFreqEnvelope : public MVAmpEnvelope
{
public:

    MVFreqEnvelope(const EnvelopeData & , int &);
    void reset();
    static inline int getSustainPoint(int i){return envFreqData[i].sustainPoint;}
    float nextFrame();
    void startRelease();

    static MVAmpEnvelope::EnvelopeData envFreqData[NB_OSC];
    static int totalLength(int);
    static int sustainLength(int);
    static int releaseLength(int);

private:

 };

#endif // MVFREQENVELOPE_H
