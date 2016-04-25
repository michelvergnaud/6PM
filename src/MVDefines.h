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

#ifndef MVDEFINES_H
#define MVDEFINES_H

#define __UNIX_JACK__

#include <math.h>
#define FIRST_NOTE 21 //A0
#define NB_NOTES 88

#define NB_OSC 6
#define NB_PREDEF_ALGOS 32
#define NB_ENV_PTS 6

#define M2PI 2.0f*M_PI
#define TWELFTH_ROOT2 1.05946309436f

#define PRESET_FILE_EXT ".mvpms"
#define MIDIMAP_FILE_EXT ".mvmm"

#endif // MVDEFINES_H
