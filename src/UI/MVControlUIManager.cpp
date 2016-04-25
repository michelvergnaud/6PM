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

#include "MVControlUIManager.h"

#include "MVControllerWidget.h"
#include "MVMainFrame.h"
#include "MVPresetManager.h"


MVControlUIManager::MVControlUIManager()
{
    bMidiLearning = false;
    ctrLearning = NULL;
}
MVControlUIManager::~MVControlUIManager()
{}

void MVControlUIManager::setMidiLearning(bool b, MVControllerWidget * c)
{
    if(b || (c == ctrLearning)) // only sender can stop
    {
       bMidiLearning = b;
       ctrLearning = c;
    }
}

bool MVControlUIManager::isMidiLearning()
{
   return bMidiLearning;
}

MVControllerWidget * MVControlUIManager::controllerLearning()
{
   return ctrLearning;
}

void MVControlUIManager::processCC(unsigned char controlNum, unsigned char param)
{
    if(bMidiLearning && ctrLearning != NULL)
    {
       registerAssign(controlNum, ctrLearning);
       bMidiLearning=false;
       ctrLearning->stopWaitingMessage();
       ctrLearning = NULL;
       return;
    }
    MVControlManager::processCC(controlNum, param);
    Globals::frame->setPresetModified(true);
    Globals::presetManager->setModified(true);
}
void MVControlUIManager::updateControlWidgets()
{
    for(int i = 0; i<controllers.size();i++)
        ((MVControllerWidget * )controllers.at(i))->updateWidget();
}
bool MVControlUIManager::loadMidiMap(QString name)
{
    if(MVControlManager::loadMidiMap(name))
    {
        updateControlWidgets();
        return true;
    }
    return false;
}
