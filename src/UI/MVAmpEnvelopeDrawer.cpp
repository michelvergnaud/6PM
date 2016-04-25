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

#include "UI/MVAmpEnvelopeDrawer.h"

#include "UI/MVSpinBoxAction.h"
#include "UI/MVMainFrame.h"

#include <QSpinBox>
#include <QMenu>

MVAmpEnvelopeDrawer::MVAmpEnvelopeDrawer(AmpEnvelopeData &ed, int & n) : MVEnvelopeDrawer(n), envData(ed)
{
}

void MVAmpEnvelopeDrawer::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    preparePaint(p);

    // Y axis
    for(int i=0;i<=5;i++)
    {
        float ay = i*h/5.0;
        p.setPen(QColor::fromRgb(32,32,32));
        p.drawLine(0,ay,w,ay);
        p.setPen(Qt::lightGray);
        QString label = QString::number(1.0 - 0.2f*i);
        p.drawText(QRectF(-textW, ay-textH/2, textW-1,textH),Qt::AlignRight,label);
    }

    int i = 0; // nb of data points
    int j = 0; // nb of graphic points

    while (i< NB_ENV_PTS)
    {
        x += w * (float)envData.points[i].frame / totalLength;
        if(numOsc >=0)
            y = h*(1.0 - (float)envData.points[i].value * Globals::presetManager->getCurrentPreset()->getOscData(numOsc)->amp);
        else
            y = h*(1.0 - (float)envData.points[i].value);
        pts[j] = QPointF(x,y);
        if(i == envData.sustainPoint)
        {
            xSustainLine = x;
            j++;
            x = w * maxSustainPointTime / totalLength;
            pts[j] = QPointF(x,y);
        }
        i++;
        j++;
    }
    pts[j]=QPoint(0,h);
    p.setBrush(QColor::fromRgb(156,156,64));

    for(int i=0;i<j+1;i++)
        pts[i].rx() *= zoomX;

    p.drawPolygon(pts, j+1);

    if(envData.sustainPoint>=0)
    {
        p.setPen(Qt::red);
        p.drawLine(xSustainLine*zoomX,0,xSustainLine*zoomX,h);
    }
    p.setPen(Qt::blue);
    x = w * (float)maxSustainPointTime*zoomX / totalLength;
    p.drawLine(x,0,x,h);

}
void MVAmpEnvelopeDrawer::contextMenu(QPoint)
{
    sustainPointAction->getSpinBox()->setValue(envData.sustainPoint+1);
    pasteAction->setEnabled( envelopeClipboard.typeEnv != -1);
    menu->popup(QCursor::pos());
}
void MVAmpEnvelopeDrawer::sustainPointChanged(int n)
{
    envData.sustainPoint = n-1;
    if(n>0)
        envData.loop = false;
    Globals::frame->updateUIs();
}

void MVAmpEnvelopeDrawer::copyEnvelope()
{
    //envelopeClipboard.numOsc = numOsc;
    envelopeClipboard.typeEnv = 0;
    envelopeClipboard.ampData = envData;
}
void MVAmpEnvelopeDrawer::pasteEnvelope()
{
    if( envelopeClipboard.typeEnv > -1)
    {
        if(envelopeClipboard.typeEnv == 0) // Amp
            envData = envelopeClipboard.ampData;
        else // Freq
        {
            for(int i=1;i<NB_ENV_PTS;i++)
                envData.points[i].frame = envelopeClipboard.freqData.points[i].frame;
            envData.loop = envelopeClipboard.freqData.loop;
            envData.sustainPoint = envelopeClipboard.freqData.sustainPoint;
        }
        Globals::frame->updateAmpEnvelope(numOsc);
    }
}
