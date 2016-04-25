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

#ifndef MVFREQENVELOPE_UI_H
#define MVFREQENVELOPE_UI_H

#include "MVEnvelope_UI.h"

#include "Synth/MVFreqEnvelope.h"

class QRadioButton;

class MVFreqEnvelope_UI : public MVEnvelope_UI
{

public:
    MVFreqEnvelope_UI(FreqEnvelopeData & , int);
    void updateUI();
    void connectWidgets();
    void disconnectWidgets();

protected:
    void setPointTime(int);
    void setPointValue(int);
    void cbLoopToggled(bool);
    void cbUseToggled(bool);

private:
    void updateLabels();
    MVCheckBox * cbUse;
    FreqEnvelopeData & envData;
};

#endif // MVFREQENVELOPE_UI_H
