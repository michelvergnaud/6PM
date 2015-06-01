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

#include "MVControlManager.h"

#include "MVController.h"

MVControlManager::MVControlManager()
{
    bMidiLearning = false;
    ctrLearning = NULL;
}

void MVControlManager::processCC(unsigned char controlNum, unsigned char param)
{
    if(bMidiLearning && ctrLearning != NULL)
    {
       registerAssign(controlNum, ctrLearning);
       bMidiLearning=false;
       ctrLearning->stopWaitingMessage();
       ctrLearning = NULL;
       return;
    }
    QList<MVController *> list = getControllers(controlNum);
    for (int i = 0; i < list.size(); ++i)
        list.at(i)->setParamValue(list.at(i)->isMidiCCInverted() ? 127-param : param);
}

void MVControlManager::removeAssign(MVController* controller)
{
    assignMap.remove(assignMap.key(controller),controller);
    controller->setState(MVController::NOASSIGN);
}

void MVControlManager::registerAssign(int ctrlNum, MVController* controller)
{
    removeAssign(controller);
    if(ctrlNum > 0)
    {
        assignMap.insert(ctrlNum, controller);
        controller->setState(MVController::ASSIGNED);
    }
}

QList<MVController*> MVControlManager::getControllers(int ctrlNum)
{
    return assignMap.values(ctrlNum);
}

int MVControlManager::getAssignNum( MVController * controller) const
{
    return assignMap.key(controller);
}

 void MVControlManager::setMidiLearning(bool b, MVController * c)
 {
     if(b || (c == ctrLearning)) // only sender can stop
     {
        bMidiLearning = b;
        ctrLearning = c;
     }
 }

bool MVControlManager::isMidiLearning()
{
    return bMidiLearning;
}

MVController * MVControlManager::controllerLearning()
{
    return ctrLearning;
}
