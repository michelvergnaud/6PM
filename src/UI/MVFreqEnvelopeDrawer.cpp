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

#include "MVFreqEnvelopeDrawer.h"

#include "MVSpinBoxAction.h"
#include "MVMainFrame.h"

#include <QSpinBox>
#include <QMenu>

MVFreqEnvelopeDrawer::MVFreqEnvelopeDrawer(FreqEnvelopeData &ed, int & n) : MVEnvelopeDrawer(n), envData(ed)
{
    zoomY = 1;
}

void MVFreqEnvelopeDrawer::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    preparePaint(p);
    p.save();
    p.translate(0,h/2);

    int maxY = 72/zoomY;
    float yStep = 1.0;
    float yStepPixel = h/(2.0*maxY);
    if(yStepPixel < 0.001)yStepPixel = 0.001;
    int factor = 2;
    while(yStepPixel < textH)
    {
        yStepPixel *= factor;
        yStep *= factor;
        factor = (factor == 2 ? 3 : 2);
    }

    QRect rect = p.fontMetrics().boundingRect(envData.absolute ? "00.00" : "+00");
    textW = rect.width();
    textH =  p.fontMetrics().ascent();

    p.setPen(QColor::fromRgb(64,64,64));
    p.drawLine(0,0,w,0);
    p.setPen(Qt::lightGray);

    QString label;
    if(envData.absolute)
        label = "1k";
    else
        label = "0";
    p.drawText(-p.fontMetrics().width(label),textH/2,label);

    float labelNum;

    // Y axis down
    float ay = yStepPixel;
    int i = 1;
    while(ay <= h/2)
    {
        p.setPen(QColor::fromRgb(32,32,32));
        p.drawLine(0,ay,w,ay);
        labelNum = i*yStep;
        if(envData.absolute)
        {
            labelNum = 0.1*round(10000.0 * pow(TWELFTH_ROOT2,-labelNum)) ;
            label = QString::number(labelNum,'f',1) ;
        }
        else
            label = QString::number(-labelNum);
        textW = p.fontMetrics().width(label);
        p.setPen(Qt::lightGray);
        p.drawText(-textW, ay+textH/2,label);
        i++;
        ay = i* yStepPixel;
    }
    // Up
    ay = -yStepPixel;
    i = 1;
    while(ay >= -h/2-2)
    {
        p.setPen(QColor::fromRgb(32,32,32));
        p.drawLine(0,ay,w,ay);
        labelNum = i*yStep;
        if(envData.absolute)
        {
            int nbDigits = 0;
            if(yStep <= 1)
                nbDigits = 2;
            if(yStep <= 12)
                nbDigits = 1;
            //labelNum = 1000*pow(TWELFTH_ROOT2,labelNum) ;
            labelNum = 0.1*round(10000.0 * pow(TWELFTH_ROOT2,labelNum)) ;
            label = QString::number((labelNum/1000),'f',nbDigits)+"k" ;
        }
        else
            label = QString::number(i*yStep);
        textW = p.fontMetrics().width(label);
        p.setPen(Qt::lightGray);
        p.drawText(-textW, ay+textH/2,label);
        ay -= yStepPixel;
        i++;
    }

    // Curve
    p.setPen(Qt::darkCyan);
    if(! envData.used)
    {
        if(numOsc >=0)
            y = -(h/144.0)*log(Globals::presetManager->getCurrentPreset()->getOscData(numOsc)->freqratio * Globals::presetManager->getCurrentPreset()->getOscData(numOsc)->detune)/log(TWELFTH_ROOT2);
        else
            y = -(h/144.0)*log(Globals::presetManager->getCurrentPreset()->getFilterData()->fRatio)/log(TWELFTH_ROOT2);
        p.drawLine(0,y,w,y);
        p.restore();
        return;
    }

    i = 0; // nb of data points
    int j = 0; // nb of graphic points

    while (i< NB_ENV_PTS)
    {
        x += w * (float)envData.points[i].frame / totalLength;
        if(numOsc >=0)
            y = -(h/144.0)*log(envData.points[i].value * Globals::presetManager->getCurrentPreset()->getOscData(numOsc)->freqratio * Globals::presetManager->getCurrentPreset()->getOscData(numOsc)->detune)/log(TWELFTH_ROOT2);
        else
            y = -(h/144.0)*log(envData.points[i].value * Globals::presetManager->getCurrentPreset()->getFilterData()->fRatio)/log(TWELFTH_ROOT2);

        pts[j] = QPointF(x,y);
        if(i == envData.sustainPoint)
        {
            xSustainLine = x;
            j++;
            x = w * maxSustainPointTime / totalLength;
            pts[j] = QPointF(x,y);
        }
        if(i == NB_ENV_PTS-1 && ! envData.loop)
        {
            j++;
            pts[j] = QPoint(w,y);
        }
        i++;
        j++;
    }

    for(int i=0;i<j+1;i++)
    {
        pts[i].rx() *= zoomX;
        pts[i].ry() *= zoomY;
    }
    p.setBrush(QColor::fromRgb(156,156,64));
    p.drawPolyline(pts, j);

    p.restore();
    if(envData.sustainPoint > -1)
    {
        p.setPen(Qt::red);
        p.drawLine(xSustainLine*zoomX,0,xSustainLine*zoomX,h);
    }
    p.setPen(Qt::blue);
    x = w * (float)maxSustainPointTime * zoomX / totalLength;
    p.drawLine(x,0,x,h);
}
void MVFreqEnvelopeDrawer::contextMenu(QPoint)
{
    sustainPointAction->getSpinBox()->setValue(envData.sustainPoint+1);
    pasteAction->setEnabled( ! (envelopeClipboard.typeEnv > -1));
    menu->popup(QCursor::pos());
}
void MVFreqEnvelopeDrawer::sustainPointChanged(int n)
{
    envData.sustainPoint = n-1;
    if(n>0)
        envData.loop = false;
    Globals::frame->updateFreqEnvelope(numOsc);
}

void MVFreqEnvelopeDrawer::copyEnvelope()
{
    envelopeClipboard.typeEnv = 1;
    envelopeClipboard.freqData = envData;
}
void MVFreqEnvelopeDrawer::pasteEnvelope()
{

    if( envelopeClipboard.typeEnv > -1)
    {
        if(envelopeClipboard.typeEnv == 1) // Freq
        {
            envData = envelopeClipboard.freqData;
        }
        else // Amp or filter
        {
            for(int i=1;i<NB_ENV_PTS;i++)
                envData.points[i].frame = envelopeClipboard.ampData.points[i].frame;
            envData.loop = envelopeClipboard.ampData.loop;
            envData.sustainPoint = envelopeClipboard.ampData.sustainPoint;
        }
        Globals::frame->updateFreqEnvelope(numOsc);
    }
}
