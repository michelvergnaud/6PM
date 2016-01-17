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

#include "MVEnvelope_ui.h"

#include "MVDial.h"
#include "MVEnvelopeDrawer.h"
#include "MVFrame.h"
#include "MVCheckBox.h"
#include "MVUtils.h"

#include <QGridLayout>
#include <QPainter>

#include <math.h>

MVEnvelope_UI::MVEnvelope_UI(MVAmpEnvelope::EnvelopeData &ed, int n, DisplayMode m) : envData(ed), numOsc(n)
{
    numOsc = n;
    displayMode = m;
    QWidget * dialsWidget = new QWidget(this);
    QGridLayout * dialslayout = new QGridLayout();
    dialslayout->setSpacing(0);
    dialslayout->setContentsMargins(1,1,1,1);
    cbLoop = new MVCheckBox(":/images/loopOn",":/images/loopOff");
    cbLoop->setMinimumSize(20,20);
    cbUse = new MVCheckBox(":/images/usedOn",":/images/usedOff");
    cbUse->setMinimumSize(10,10);

    if(displayMode == FREQ)
        dialslayout->addWidget(cbUse, 1, 0);

    for(int i=0;i<NB_ENV_PTS;i++)
    {
        dialsTime[i] = new MVDial(this);
        dialsTime[i]->setMaximum(nbTimeValues-1);
        dialsTime[i]->setMinimum(0);
        dialsTime[i]->setMinimumSize(10,10);

        dialsValue[i] = new MVDial(this,displayMode == AMP ? 50 : 0);
        dialsValue[i]->setMinimum(displayMode == AMP ? 0 : -240); // Amp in % Freq -2 to 2 octaves
        dialsValue[i]->setMaximum(displayMode == AMP ? 100 : 240);
        dialsValue[i]->setMinimumSize(10,10);

        if(i==envData.sustainPoint)
        {
            dialsValue[i]->setMainColor(QColor::fromRgb(92,0,0));
            dialsTime[i]->setMainColor(QColor::fromRgb(92,0,0));
        }

        if(i>0 || displayMode == AMP)
            dialslayout->addWidget(dialsTime[i], 1, i);

        if (i<NB_ENV_PTS-1)
            dialslayout->addWidget(dialsValue[i], 0, i);
        else
            dialslayout->addWidget(cbLoop, 0, i);

        connect(dialsTime[i] , SIGNAL(valueChanged(int)),this,SLOT(setPointTime(int)));
        connect(dialsValue[i] , SIGNAL(valueChanged(int)),this,SLOT(setPointValue(int)));
    }
    saveSustainPoint = envData.sustainPoint;
    connect(cbLoop,SIGNAL(toggled(bool)),this,SLOT(cbLoopToggled(bool)));
    connect(cbUse,SIGNAL(toggled(bool)),this,SLOT(cbUseToggled(bool)));

    dialsWidget->setLayout(dialslayout);
    envDrawer = new MVEnvelopeDrawer(ed, numOsc, (MVEnvelopeDrawer::DisplayMode)displayMode);

    QVBoxLayout * vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0,0,0,0);
    this->setLayout(vlayout);
    vlayout->addWidget(envDrawer,10);
    vlayout->addWidget(dialsWidget,7);
}

void MVEnvelope_UI::updateUI()
{
    for(int i=0;i<NB_ENV_PTS;i++)
    {
        if(i>0 || displayMode == AMP)      
                     dialsTime[i]->setValue(indexOfTime(framesTomsec(envData.points[i].frame)));
        if (i<NB_ENV_PTS-1)
            dialsValue[i]->setValue(displayMode==AMP ? envData.points[i].value*100 : round(10*log(envData.points[i].value))/log(TWELFTH_ROOT2));

        dialsValue[i]->setMainColor(i == envData.sustainPoint ? QColor::fromRgb(92,0,0) : Qt::black);
        dialsTime[i]->setMainColor(i == envData.sustainPoint ? QColor::fromRgb(92,0,0) : Qt::black);
    }
    cbLoop->setChecked(envData.loop);
    cbUse->setChecked(envData.used);
    updateLabels();
    envDrawer->update();
}
void MVEnvelope_UI::updateDrawer()
{
    envDrawer->update();
}
void MVEnvelope_UI::updateLabels()
{
    for(int i=0;i<NB_ENV_PTS;i++)
    {
        float num = timeValues[dialsTime[i]->value()];
        int nbDigits = 0;
        if(num < 100) nbDigits = 3;
        else if(num < 1000) nbDigits = 2;
        else if(num < 10000) nbDigits = 1;
        dialsTime[i]->setLabel(QString::number(num/1000,'f',nbDigits));

        if(displayMode == AMP)
            dialsValue[i]->setLabel(QString::number((float)dialsValue[i]->value()/100.0,'f',2));
        else
            dialsValue[i]->setLabel(QString::number(dialsValue[i]->value()/10.0,'f',1));
    }
}
void MVEnvelope_UI::cbLoopToggled(bool b)
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
    updateUI();
}

void MVEnvelope_UI::cbUseToggled(bool b)
{
    envData.used = b;
    envDrawer->update();
    updateUI();
}
void MVEnvelope_UI::setPointTime(int n)
{
    QObject * sender = QObject::sender();
    int p = 0;
    while (dialsTime[p] != sender)
        p++;
    envData.points[p].frame = msecToFrames(timeValues[n]);
    Globals::frame->setTimeScales();
    updateLabels();
}

int MVEnvelope_UI::indexOfTime(float t)
{
    bool ok = false;
    int id;
    int ifin;
    int im;
    id = 0;
    ifin = nbTimeValues;
    while(! ok && ((ifin - id) > 1))
    {
        im = (id + ifin)/2;
        ok = (timeValues[im] == t);
        if(timeValues[im] > t) ifin = im;
        else id = im;
    }
    return id;
}

void MVEnvelope_UI::setPointValue(int v)
{
    QObject * sender =QObject::sender();
    int p = 0;
    while (dialsValue[p] != sender)
        p++;
    envData.points[p].value = (displayMode == AMP ? (float)v/100.0 : pow(TWELFTH_ROOT2, (float)v/10.0));
    envDrawer->update();
    updateLabels();
}

float * MVEnvelope_UI::timeValues;
int MVEnvelope_UI::nbTimeValues = 64;
