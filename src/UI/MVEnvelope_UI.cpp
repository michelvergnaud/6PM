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

#include "MVEnvelope_UI.h"

#include "MVDial.h"
#include "MVEnvelopeDrawer.h"
#include "MVMainFrame.h"
#include "MVCheckBox.h"
#include "MVUtils.h"
#include "Synth/MVControlNames.h"
#include "Synth/MVFreqEnvelope.h"

#include <QGridLayout>
#include <QPainter>
#include <math.h>

MVEnvelope_UI::MVEnvelope_UI(int n)
{
    numOsc = n;
    dialsWidget = new QWidget(this);
    dialslayout = new QGridLayout();
    dialslayout->setSpacing(0);
    dialslayout->setContentsMargins(1,1,1,1);

    for(int i=0;i<NB_ENV_PTS;i++)
    {
        dialsTime[i] = new MVDial(this,0,"");
        dialsTime[i]->setMaximum(MVFreqEnvelope::nbTimeValues-1);
        dialsTime[i]->setMinimum(0);
        dialsTime[i]->setMinimumSize(10,10);

        dialsValue[i] = new MVDial(this,0,"");
        dialsValue[i]->setMinimumSize(10,10);

        if (i<NB_ENV_PTS-1)
            dialslayout->addWidget(dialsValue[i], 0, i);
        else
        {
            dialsValue[i]->setVisible(false);
        }
    }

    dialsWidget->setLayout(dialslayout);

    vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0,0,0,0);
    this->setLayout(vlayout);
    connectWidgets();
}

void MVEnvelope_UI::connectWidgets()
{
    for(int i=0;i<NB_ENV_PTS;i++)
    {
        connect(dialsTime[i] , SIGNAL(valueChanged(int)),this,SLOT(setPointTime(int)));
        connect(dialsValue[i] , SIGNAL(valueChanged(int)),this,SLOT(setPointValue(int)));
    }
}

void MVEnvelope_UI::disconnectWidgets()
{
    for(int i=0;i<NB_ENV_PTS;i++)
    {
        dialsTime[i]->disconnect(this);
        dialsValue[i]->disconnect(this);
    }
}

void MVEnvelope_UI::updateDrawer()
{
    envDrawer->update();
}
void MVEnvelope_UI::updateLabels()
{
    for(int i=0;i<NB_ENV_PTS;i++)
    {
        float num = MVFreqEnvelope::timeValues[dialsTime[i]->value()];
        int nbDigits = 0;
        if(num < 100) nbDigits = 3;
        else if(num < 1000) nbDigits = 2;
        else if(num < 10000) nbDigits = 1;
        dialsTime[i]->setLabel(QString::number(num/1000,'f',nbDigits));
    }
}
void MVEnvelope_UI::setPresetModified(bool b)
{
    Globals::presetManager->setModified(true);
    Globals::frame->setPresetModified(b);
}
int MVEnvelope_UI::indexOfTime(float t)
{
    bool ok = false;
    int id;
    int ifin;
    int im;
    id = 0;
    ifin = MVFreqEnvelope::nbTimeValues;
    while(! ok && ((ifin - id) > 1))
    {
        im = (id + ifin)/2;
        ok = (MVFreqEnvelope::timeValues[im] == t);
        if(MVFreqEnvelope::timeValues[im] > t) ifin = im;
        else id = im;
    }

    return id;
}
