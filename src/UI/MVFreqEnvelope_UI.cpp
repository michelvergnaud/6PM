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

#include "MVFreqEnvelope_UI.h"

#include "MVCheckBox.h"
#include "MVDial.h"
#include "MVFreqEnvelopeDrawer.h"
#include "MVMainFrame.h"
#include "Synth/MVControlNames.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QRadioButton>

MVFreqEnvelope_UI::MVFreqEnvelope_UI(FreqEnvelopeData &ed, int n) : MVEnvelope_UI(n), envData(ed)
{
    cbLoop = new MVCheckBox(this, ":/images/loopOn",":/images/loopOff",QString(CTRN_envFreq1loopmidiCC).arg(n));
    cbLoop->setMinimumSize(20,20);
    dialslayout->addWidget(cbLoop, 0, NB_ENV_PTS-1);

    for(int i=0;i<NB_ENV_PTS;i++)
    {
        dialsTime[i]->setName(QString(CTRN_envFreq12midiCC_frame).arg(n).arg(i));
        dialsValue[i]->setName(QString(CTRN_envFreq12midiCC_value).arg(n).arg(i));
        dialsValue[i]->setMinimum(-48);
        dialsValue[i]->setMaximum(48);
        dialsValue[i]->setMinimumSize(10,10);

        if(i == envData.sustainPoint)
        {
            dialsValue[i]->setMainColor(QColor::fromRgb(92,0,0));
            dialsTime[i]->setMainColor(QColor::fromRgb(92,0,0));
        }
        if(i>0)
            dialslayout->addWidget(dialsTime[i], 1, i);
    }
    saveSustainPoint = envData.sustainPoint;

    cbUse = new MVCheckBox(this, ":/images/usedOn",":/images/usedOff",QString(CTRN_envFreq1usedmidiCC).arg(n));
    cbUse->setMinimumSize(10,10);
    cbUse->setChecked(true);

    dialslayout->addWidget(cbUse, 1, 0);

    envDrawer = new MVFreqEnvelopeDrawer(ed, numOsc);
    vlayout->addWidget(envDrawer,10);
    vlayout->addWidget(dialsWidget,7);

    connectWidgets();
}
void MVFreqEnvelope_UI::connectWidgets()
{
    MVEnvelope_UI::connectWidgets();
    connect(cbLoop,SIGNAL(toggled(bool)),this,SLOT(cbLoopToggled(bool)));
    connect(cbUse,SIGNAL(toggled(bool)),this,SLOT(cbUseToggled(bool)));
}
void MVFreqEnvelope_UI::disconnectWidgets()
{
    MVEnvelope_UI::disconnectWidgets();
    cbUse->disconnect(this);
    cbLoop->disconnect(this);
}
void MVFreqEnvelope_UI::updateUI()
{
    disconnectWidgets();
    for(int i=0;i<NB_ENV_PTS;i++)
    {
        if(i>0)
            dialsTime[i]->setValue(indexOfTime(framesTomsec(envData.points[i].frame)));
        if(i<NB_ENV_PTS-1)
            dialsValue[i]->setValue(round(log(envData.points[i].value)/log(TWELFTH_ROOT2)));
        dialsValue[i]->setMainColor(i == envData.sustainPoint ? QColor::fromRgb(92,0,0) : Qt::black);
        dialsTime[i]->setMainColor(i == envData.sustainPoint ? QColor::fromRgb(92,0,0) : Qt::black);
    }
    cbLoop->setChecked(envData.loop);
    cbUse->setChecked(envData.used);
    updateLabels();
    envDrawer->update();
    for(int i=0;i<NB_ENV_PTS;i++)
    {
        dialsTime[i]->setEnabled(envData.used);
        dialsValue[i]->setEnabled(envData.used);
    }
    cbLoop->setEnabled(envData.used);

    connectWidgets();
}
void MVFreqEnvelope_UI::updateLabels()
{
    MVEnvelope_UI::updateLabels();
    for(int i=0;i<NB_ENV_PTS;i++)
    {
        if(envData.absolute)
        {
            float f = pow(TWELFTH_ROOT2,(float)dialsValue[i]->value());
            int nbDigits = (f > 1 ? 1 : 2);
            QString label = "x\n"+QString::number(f,'f',nbDigits);
            dialsValue[i]->setLabel(label);
        }
        else
            dialsValue[i]->setLabel(QString::number(dialsValue[i]->value(),'f',1));
    }
}
void MVFreqEnvelope_UI::setPointTime(int n)
{
    QObject * sender = QObject::sender();
    int p = 0;
    while (dialsTime[p] != sender)
        p++;
    envData.points[p].frame = msecToFrames(MVFreqEnvelope::timeValues[n]);
    Globals::frame->setTimeScales();
    updateLabels();
    setPresetModified(true);
}
void MVFreqEnvelope_UI::setPointValue(int v)
{
    QObject * sender =QObject::sender();
    int p = 0;
    while (dialsValue[p] != sender)
        p++;

    envData.points[p].value = pow(TWELFTH_ROOT2, (float)v);
    envDrawer->update();
    updateLabels();
    setPresetModified(true);
}
void MVFreqEnvelope_UI::cbLoopToggled(bool b)
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
void MVFreqEnvelope_UI::cbUseToggled(bool b)
{
    envData.used = b;
    envDrawer->update();
    for(int i=0;i<NB_ENV_PTS;i++)
    {
        dialsTime[i]->setEnabled(b);
        dialsValue[i]->setEnabled(b);
    }
    cbLoop->setEnabled(b);
    setPresetModified(true);
    updateUI();
}
