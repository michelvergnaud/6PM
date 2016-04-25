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

#ifndef MVEnvelope_UI_H
#define MVEnvelope_UI_H

#include "MVDefines.h"
#include "Synth/MVAmpEnvelope.h"
#include "MVBackground.h"

#include <QWidget>

class MVDial;
class MVEnvelopeDrawer;
class MVCheckBox;
class QGridLayout;
class QVBoxLayout;

class MVEnvelope_UI : public MVBackground
{
Q_OBJECT

public:
    MVEnvelope_UI(int);
    virtual void updateUI() = 0;
    void updateLabels();
    void connectWidgets();
    void disconnectWidgets();

    void updateDrawer();
    MVEnvelopeDrawer * getDrawer(){return envDrawer;}

    MVDial * dialsTime[NB_ENV_PTS];
    MVDial * dialsValue[NB_ENV_PTS];

protected:
    int numOsc;
    MVCheckBox * cbLoop;
    int saveSustainPoint;
    QGridLayout * dialslayout;
    QVBoxLayout * vlayout;
    QWidget * dialsWidget;
    MVEnvelopeDrawer * envDrawer;

    int indexOfTime(float);
    void setPresetModified(bool);


protected slots:
    virtual void setPointTime(int f) = 0;
    virtual void setPointValue(int v) = 0;
    virtual void cbLoopToggled(bool) = 0;
    virtual void cbUseToggled(bool) = 0;
};

#endif // MVEnvelope_UI_H
