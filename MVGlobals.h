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

#ifndef GLOBALS_H
#define GLOBALS_H

#include "MVDefines.h"

#include <QString>

class MVFrame;
class MVControlManager;
class MVPlayer;

class Globals
{
public:
    static QString userDir;
    static QString userPresetsDir;
    static QString userMidiMapsDir;
    static MVFrame * frame;
    static MVControlManager * controlmanager;
    static MVPlayer * player;
};

#endif // GLOBALS_H
