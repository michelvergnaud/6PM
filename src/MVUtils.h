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

#ifndef MVUTILS_H
#define MVUTILS_H

#include <pthread.h>
#include <semaphore.h>

float msecToFrames(int msec);
float framesTomsec(int frames);

bool startThread(pthread_t *pth, void *(*thread_fn)(void*), void *arg, bool schedfifo, int rtprio, char priodec, bool create_detached);

#endif // MVUTILS_H
