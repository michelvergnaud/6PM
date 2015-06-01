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

#include "MVUtils.h"

#include "MVPlayer.h"

float msecToFrames(int msec){return (float)msec * MVPlayer::getSampleRate() / 1000.0;}
float framesTomsec(int frames){return 1000.0 * (float)frames / MVPlayer::getSampleRate() ;}

bool startThread(pthread_t *pth, void *(*thread_fn)(void*), void *arg, bool schedfifo, int rtprio, char priodec, bool create_detached)
{
    pthread_attr_t attr;
    int chk;
    bool outcome = false;
    bool retry = true;
    while (retry)
    {
        if (!(chk = pthread_attr_init(&attr)))
        {
            if(create_detached)
            {
               chk = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            }
            if(!chk)
            {
                if (schedfifo)
                {
                    if ((chk = pthread_attr_setschedpolicy(&attr, SCHED_FIFO)))
                    {
                        std::cout << "Failed to set SCHED_FIFO policy in thread attribute" << std::endl;
                        schedfifo = false;
                        continue;
                    }
                    if ((chk = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)))
                    {
                        std::cout << "Failed to set inherit scheduler thread attribute" << std::endl;
                        schedfifo = false;
                        continue;
                    }
                    sched_param prio_params;
                    int prio = rtprio;
                    if (priodec)
                        prio -= priodec;
                    prio_params.sched_priority = (prio > 0) ? prio : 0;
                    if ((chk = pthread_attr_setschedparam(&attr, &prio_params)))
                    {
                        std::cout << "Failed to set thread priority attribute" << " error "<< chk <<std::endl;
                        schedfifo = false;
                        continue;
                    }
                }

                if (!(chk = pthread_create(pth, &attr, thread_fn, arg)))
                {
                    outcome = true;
                    break;
                }
                else if (schedfifo)
                {
                    schedfifo = false;
                    continue;
                }
                outcome = false;
                break;
            }
            else
                std::cout << "Failed to set thread detach state: " << std::endl;
            pthread_attr_destroy(&attr);
        }
        else
            std::cout <<  "Failed to initialise thread attributes: " << std::endl;

        if (schedfifo)
        {
            std::cout << "Failed to start thread (sched_fifo): " << std::endl;
            schedfifo = false;
            continue;
        }
        std::cout << "Failed to start thread (sched_other): " << std::endl;

        outcome = false;
        break;
    }
    return outcome;
}

