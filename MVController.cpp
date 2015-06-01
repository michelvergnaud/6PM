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

#include "MVController.h"
#include "MVGlobals.h"
#include "MVCheckBoxAction.h"
#include "MVControlManager.h"

#include <QTimer>
#include <QAction>
#include <QMenu>

MVController::MVController()
{
    state = NOASSIGN;
    bMidiCCInverted = false;
    bAssignable = true;

    nbFlash = 30;
    timer.setInterval(100);
    bOn = false;
    menu = new QMenu();

    midiLearnAction = new QAction("MidiLearn", 0);
    removeAssignAction = new QAction("Remove assignment", 0);
    cbInvertAction = new MVCheckBoxAction("Invert midi CC");

    menu->addAction("Not assigned");
    menu->addAction(removeAssignAction);
    menu->addAction(cbInvertAction);
    menu->addAction(midiLearnAction);
}
MVController::~MVController()
{
    delete menu;
    delete midiLearnAction;
    delete removeAssignAction;
    delete cbInvertAction;
}
bool MVController::isAssigned()
{
    return getAssignNumber() > 0;
}

int MVController::getAssignNumber()
{
    return Globals::controlmanager->getAssignNum(this);
}
void MVController::setAssignNumber(int n, bool invert)
{
    bMidiCCInverted = invert;
    if(n>0 && n<=127)
        state = ASSIGNED;
    else
        state = NOASSIGN;
}

void MVController::displayMenu()
{
    if(! bAssignable) return;

    if(isAssigned())
    {
        menu->actions().at(0)->setText(QString("Assigned to : control %1").arg(getAssignNumber()));
        menu->actions().at(1)->setEnabled(true);
        menu->actions().at(2)->setEnabled(true);
    }
    else
    {
        menu->actions().at(0)->setText("Not assigned");
        menu->actions().at(1)->setEnabled(false);
        menu->actions().at(2)->setEnabled(false);
    }
    menu->popup(QCursor::pos());
}

void MVController::startWaitingMessage()
{
    bOn = ! bOn;
    if(bOn)
        switchOnRed();
    else
        switchOff();
    nbFlash--;
    if(nbFlash==0)
        stopWaitingMessage();
}

void MVController::midiLearn()
{
    if (! Globals::controlmanager->isMidiLearning() )
    {
        Globals::controlmanager->setMidiLearning(true, this);
        startWaitingMessage();
        nbFlash = 30;
        timer.start();
    }
}

void MVController::stopWaitingMessage()
{
    timer.stop();
    if(getAssignNumber() > 0)
        switchOnGreen();
    else
        switchOff();

    Globals::controlmanager->setMidiLearning(false, this);
}

void MVController::switchOff()
{
    state = NOASSIGN;
    updateWidget();
}

void MVController::switchOnRed()
{
    state = WAITING;
    updateWidget();
}
void MVController::switchOnGreen()
{
    state = ASSIGNED;
    updateWidget();
}
void MVController::removeAssign()
{
    Globals::controlmanager->removeAssign(this);
    switchOff();
}
void MVController::invertMidiCC(bool b)
{
    bMidiCCInverted = b;
}
