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

#include "MVControllerWidget.h"
#include "MVGlobals.h"
#include "UI/MVCheckBoxAction.h"
#include "UI/MVControlUIManager.h"

#include <QTimer>
#include <QAction>
#include <QMenu>

MVControllerWidget::MVControllerWidget(QString name) : MVController(name)
{
    state = NOASSIGN;
    bMidiCCInverted = false;
    bAssignable = true;

    nbFlash = 30;
    timer.setInterval(100);
    bOn = false;
    menu = new QMenu();

    midiLearnAction = new QAction(QObject::tr("Midi Learn"), 0);
    removeAssignAction = new QAction(QObject::tr("Remove assignment"), 0);
    cbInvertAction = new MVCheckBoxAction(QObject::tr("Invert midi CC"));

    menu->addAction(QObject::tr("Not assigned"));
    menu->addAction(removeAssignAction);
    menu->addAction(cbInvertAction);
    menu->addAction(midiLearnAction);
    if(bAssignable) Globals::controlmanager->addController(this);
}
MVControllerWidget::~MVControllerWidget()
{
    delete menu;
    delete midiLearnAction;
    delete removeAssignAction;
    delete cbInvertAction;
}
bool MVControllerWidget::isAssigned()
{
    return getAssignNumber() > 0;
}

int MVControllerWidget::getAssignNumber()
{
    return Globals::controlmanager->getAssignNum(this);
}
void MVControllerWidget::setAssignNumber(int n, bool invert)
{
    bMidiCCInverted = invert;
    if(n>0 && n<=127)
        state = ASSIGNED;
    else
        state = NOASSIGN;
}

void MVControllerWidget::displayMenu()
{
    if(! bAssignable) return;

    if(isAssigned())
    {
        menu->actions().at(0)->setText(QObject::tr("Assigned to : control %1").arg(getAssignNumber()));
        menu->actions().at(1)->setEnabled(true);
        menu->actions().at(2)->setEnabled(true);
    }
    else
    {
        menu->actions().at(0)->setText(QObject::tr("Not assigned"));
        menu->actions().at(1)->setEnabled(false);
        menu->actions().at(2)->setEnabled(false);
    }
    menu->popup(QCursor::pos());
}

void MVControllerWidget::startWaitingMessage()
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

void MVControllerWidget::midiLearn()
{
    if (! ((MVControlUIManager*)Globals::controlmanager)->isMidiLearning() )
    {
        ((MVControlUIManager*)Globals::controlmanager)->setMidiLearning(true, this);
        startWaitingMessage();
        nbFlash = 30;
        timer.start();
    }
}

void MVControllerWidget::stopWaitingMessage()
{
    timer.stop();
    if(getAssignNumber() > 0)
        switchOnGreen();
    else
        switchOff();

    ((MVControlUIManager*)Globals::controlmanager)->setMidiLearning(false, this);
}

void MVControllerWidget::switchOff()
{
    state = NOASSIGN;
    updateWidget();
}

void MVControllerWidget::switchOnRed()
{
    state = WAITING;
    updateWidget();
}
void MVControllerWidget::switchOnGreen()
{
    state = ASSIGNED;
    updateWidget();
}
void MVControllerWidget::removeAssign()
{
    Globals::controlmanager->removeAssign(this);
    switchOff();
}

