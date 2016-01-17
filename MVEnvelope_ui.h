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

#ifndef MVEnvelope_UI_H
#define MVEnvelope_UI_H

#include "MVDefines.h"
#include "MVAmpEnvelope.h"
#include "MVBackground.h"

#include <QWidget>

class MVDial;
class MVEnvelopeDrawer;
class MVCheckBox;

class MVEnvelope_UI : public MVBackground
{
Q_OBJECT
public:
    enum DisplayMode{AMP,FREQ};
    MVEnvelope_UI(MVAmpEnvelope::EnvelopeData & ed, int, DisplayMode);
    void updateUI();
    void updateDrawer();
    MVEnvelopeDrawer * getDrawer(){return envDrawer;}

    MVDial * dialsTime[NB_ENV_PTS];
    MVDial * dialsValue[NB_ENV_PTS];

    static float * timeValues;
    static int nbTimeValues;

private:
    DisplayMode displayMode;
    MVCheckBox * cbLoop;
    MVCheckBox * cbUse;
    int saveSustainPoint;
    MVEnvelopeDrawer * envDrawer;
    MVAmpEnvelope::EnvelopeData & envData;
    int indexOfTime(float);
    void updateLabels();
    int numOsc;

private slots:
    void setPointTime(int f);
    void setPointValue(int v);
    void cbLoopToggled(bool);
    void cbUseToggled(bool);
};

#endif // MVEnvelope_UI_H
