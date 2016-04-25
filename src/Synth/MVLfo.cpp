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

#include "MVLfo.h"

MVLfo::MVLfo(const LfoData &lfod, LfoFunction func) : data(lfod)
{
    lfoFunction = (LfoFunction)func;
    dt = 0;
    t = 0.0;
    phi = 0.0;
    oldFreq = data.freq;
    tempDt = 0;
    v = 0;
}
