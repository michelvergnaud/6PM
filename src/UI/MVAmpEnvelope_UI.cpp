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

#include "UI/MVAmpEnvelope_UI.h"

#include "UI/MVDial.h"
#include "UI/MVAmpEnvelopeDrawer.h"
#include "UI/MVCheckBox.h"
#include "UI/MVMainFrame.h"
#include "Synth/MVControlNames.h"
#include "Synth/MVFreqEnvelope.h"

#include <QGridLayout>

MVAmpEnvelope_UI::MVAmpEnvelope_UI(AmpEnvelopeData &ed, int n) : MVEnvelope_UI(n), envData(ed)
{
    cbLoop = new MVCheckBox(this, ":/images/loopOn",":/images/loopOff",QString(CTRN_envAmp1loopmidiCC).arg(n));
    cbLoop->setMinimumSize(20,20);
    dialslayout->addWidget(cbLoop, 0, NB_ENV_PTS-1);

    for(int i=0;i<NB_ENV_PTS;i++)
    {
        dialsTime[i]->setName(QString(CTRN_envAmp12midiCC_frame).arg(n).arg(i));
        dialsValue[i]->setName(QString(CTRN_envAmp12midiCC_value).arg(n).arg(i));
        dialsValue[i]->setDefaultValue(50);
        dialsValue[i]->setMinimum(0);
        dialsValue[i]->setMaximum(100);
        dialsValue[i]->setMinimumSize(10,10);

        if(i == envData.sustainPoint)
        {
            dialsValue[i]->setMainColor(QColor::fromRgb(92,0,0));
            dialsTime[i]->setMainColor(QColor::fromRgb(92,0,0));
        }
        dialslayout->addWidget(dialsTime[i], 1, i);
    }
    saveSustainPoint = envData.sustainPoint;
    envDrawer = new MVAmpEnvelopeDrawer(ed, numOsc);
    vlayout->addWidget(envDrawer,10);
    vlayout->addWidget(dialsWidget,7);
}
void MVAmpEnvelope_UI::connectWidgets()
{
    MVEnvelope_UI::connectWidgets();
    connect(cbLoop,SIGNAL(toggled(bool)),this,SLOT(cbLoopToggled(bool)));
}
void MVAmpEnvelope_UI::disconnectWidgets()
{
    MVEnvelope_UI::disconnectWidgets();
    cbLoop->disconnect(this);
}
void MVAmpEnvelope_UI::updateUI()
{
    disconnectWidgets();
    for(int i=0;i<NB_ENV_PTS;i++)
    {
        dialsTime[i]->setValue(indexOfTime(framesTomsec(envData.points[i].frame)));
        dialsValue[i]->setMainColor(i == envData.sustainPoint ? QColor::fromRgb(92,0,0) : Qt::black);
        dialsTime[i]->setMainColor(i == envData.sustainPoint ? QColor::fromRgb(92,0,0) : Qt::black);
    }
    cbLoop->setChecked(envData.loop);

    for(int i=0;i<NB_ENV_PTS-1;i++)
        dialsValue[i]->setValue(envData.points[i].value*100 );

    updateLabels();
    envDrawer->update();
    connectWidgets();
}
void MVAmpEnvelope_UI::updateLabels()
{
    MVEnvelope_UI::updateLabels();
    for(int i=0;i<NB_ENV_PTS;i++)
       dialsValue[i]->setLabel(QString::number((float)dialsValue[i]->value()/100.0,'f',2));
}
void MVAmpEnvelope_UI::setPointTime(int n)
{
    QObject * sender = QObject::sender();
    int p = 0;
    while (dialsTime[p] != sender)
        p++;

    envData.points[p].frame = msecToFrames(MVFreqEnvelope::timeValues[n]);
    Globals::frame->setTimeScales();
    setPresetModified(true);
    updateLabels();
    updateUI();
}
void MVAmpEnvelope_UI::setPointValue(int v)
{
    QObject * sender =QObject::sender();
    int p = 0;
    while (dialsValue[p] != sender)
        p++;
    envData.points[p].value = (float)v/100.0;
    envDrawer->update();
    updateLabels();
    setPresetModified(true);
}
void MVAmpEnvelope_UI::cbLoopToggled(bool b)
{
    if(!b)
    {
       envData.sustainPoint = saveSustainPoint;
    }
    else
    {
        saveSustainPoint = envData.sustainPoint;
        envData.sustainPoint = -1;
    }
    envData.loop = b;
    envDrawer->update();
    setPresetModified(true);
    updateUI();
}
void MVAmpEnvelope_UI::cbUseToggled(bool)
{}
