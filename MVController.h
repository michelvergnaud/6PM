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

#ifndef MVCONTROLLER_H
#define MVCONTROLLER_H

#include <QTimer>

class QMenu;
class QAction;
class MVCheckBoxAction;
class QMenu;

class MVController
{
public:
    MVController();
    ~MVController();

    enum ControllerState {NOASSIGN,ASSIGNED,WAITING};

    virtual void setAssignNumber(int,bool invert=false);
    virtual void removeAssign();
    void switchOnGreen();
    void switchOnRed();
    void switchOff();

    bool isAssigned();
    int getAssignNumber() ;
    virtual void midiLearn();
    virtual void startWaitingMessage();
    virtual void stopWaitingMessage();
    void displayMenu();
    virtual void setParamValue(int)=0;
    void invertMidiCC(bool b);
    virtual void updateWidget()=0;
    ControllerState getState() const{return state;}
    void setState(ControllerState s){state = s;}
    bool isMidiCCInverted(){return bMidiCCInverted;}

protected:
       ControllerState state;
       bool bMidiCCInverted;
       int nbFlash;
       QTimer timer;
       bool bOn;
       bool bAssignable;
       QMenu * menu;
       QAction * midiLearnAction;
       QAction * removeAssignAction;
       MVCheckBoxAction * cbInvertAction;
};

#endif // MVCONTROLLER_H
