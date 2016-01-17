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

#include "MVEnvelopeDrawer.h"

#include "MVSpinBoxAction.h"
#include "MVFrame.h"
#include "MVNote.h"

#include <math.h>

#include <QSpinBox>
#include <QPainter>
#include <QAction>
#include <QMenu>
#include <QLabel>
#include <QWheelEvent>


MVEnvelopeDrawer::MVEnvelopeDrawer(MVAmpEnvelope::EnvelopeData & ed, int & n, DisplayMode m) : envData(ed), numOsc(n)
{
    displayMode = m;
    zoomX = 1.0;
    zoomY = 1.0;
    sustainPointAction = new MVSpinBoxAction("Sustain point");
    sustainPointAction->getSpinBox()->setMaximum(NB_ENV_PTS-1);
    sustainPointAction->getSpinBox()->setMinimum(0);
    sustainPointAction->getSpinBox()->setSpecialValueText("None");

    menu = new QMenu();
    menu->addAction(sustainPointAction);
    menu->addAction("Copy",this,SLOT(copyEnvelope()));
    pasteAction = new QAction("Paste", this);
    menu->addAction(pasteAction);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
    connect(sustainPointAction->getSpinBox (), SIGNAL(valueChanged(int)),this,SLOT(sustainPointChanged(int)));
    connect(pasteAction,SIGNAL(triggered()),this,SLOT(pasteEnvelope()));
}
MVEnvelopeDrawer::~MVEnvelopeDrawer()
{
    delete sustainPointAction;
    delete menu;
}

void MVEnvelopeDrawer::setTimeScale(float tl, float s)
{
     totalLength = tl;
     maxSustainPointTime = s;
}

void MVEnvelopeDrawer::drawAmp(QPainter& p)
{
    // Y axis
    for(int i=0;i<=5;i++)
    {
        y = i*h/5.0;
        p.setPen(QColor::fromRgb(32,32,32));
        p.drawLine(0,y,w,y);
        p.setPen(Qt::lightGray);
        QString label = QString::number(1.0 - 0.2f*i);
        p.drawText(QRectF(-textW, y-textH/2, textW-1,textH),Qt::AlignRight,label);
    }

    int i = 0; // nb of data points
    int j = 0; // nb of graphic points

    while (i< NB_ENV_PTS)
    {
        x += w * (float)envData.points[i].frame / totalLength;
        y = h*(1.0 - (float)envData.points[i].value * MVNote::oscData[numOsc].amp);
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
void MVEnvelopeDrawer::drawFreq(QPainter& p)
{
    p.save();
    p.translate(0,h/2);

    int nbGrad = 48 * (int)zoomY;
    float dY = h / nbGrad;
    // Y axis
    for(int i=0;i<nbGrad;i++)
    {
        y = (i-4)*6*dY;
        p.setPen(QColor::fromRgb(32,32,32));
        p.drawLine(0,y*zoomY,w,y*zoomY);
        if(i%2==0)
        {
            p.setPen(Qt::lightGray);
            p.drawText(QRectF(-textW, y*zoomY-textH/2, textW-1,textH),Qt::AlignRight,QString::number(48/(int)zoomY-i*12/(int)zoomY));
        }
    }

    // Curve
    p.setPen(Qt::darkCyan);
    int i = 0; // nb of data points
    int j = 0; // nb of graphic points

     while (i< NB_ENV_PTS)
    {
        x += w * (float)envData.points[i].frame / totalLength;
        y = -(h/96.0)*log(envData.points[i].value * MVNote::oscData[numOsc].freqratio * MVNote::oscData[numOsc].detune)/log(TWELFTH_ROOT2);

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

void MVEnvelopeDrawer::paintEvent ( QPaintEvent *)
{
    QPainter p(this);
    p.setBackground(Qt::black);
    p.eraseRect(0,0,width(),height());
    QFont font = p.font();
    font.setPointSizeF(6);
    p.setFont(font);
    QRect rect = p.fontMetrics().boundingRect("+000");
    textW = rect.width();
    textH = rect.height();

    p.translate(textW,textH/2);
    h = height()-textH-1;
    w = width()-textW-1;
    x = 0.0;
    y = 0.0;
    xSustainLine = 0;
    xMaxSustainLine = 0;
    if(displayMode == AMP)
        drawAmp(p);
    else
        drawFreq(p);
 }

void MVEnvelopeDrawer::wheelEvent(QWheelEvent * e)
{
    int z = e->angleDelta().y();
    if( e->modifiers() & Qt::ControlModifier )
    {
        if(z > 0 && zoomX < 100)
            zoomX *= 1.5;
        if(z<0 && zoomX > 1)
            zoomX *= 0.75;
        if(zoomX > 100.0) zoomX = 100.0;
        if(zoomX < 1.0) zoomX = 1.0;
    }
    if( e->modifiers() & Qt::ShiftModifier )
    {
        if(z > 0 && zoomY < 100)
            zoomY *= 1.5;
        if(z<0 && zoomY > 1)
            zoomY *= 0.75;
        if(zoomY > 100.0) zoomY = 100.0;
        if(zoomY < 1.0) zoomY = 1.0;
    }

    update();
}

void MVEnvelopeDrawer::contextMenu(QPoint)
{
    sustainPointAction->getSpinBox()->setValue(envData.sustainPoint+1);
    pasteAction->setEnabled( ! MVAmpEnvelope::bEnvelopeClipboardEmpty);
    menu->popup(QCursor::pos());
}

void MVEnvelopeDrawer::sustainPointChanged(int n)
{
    envData.sustainPoint = n-1;
    if(n>0)
        envData.loop = false;
    Globals::frame->updateUIs();
}

void MVEnvelopeDrawer::copyEnvelope()
{
    MVAmpEnvelope::envelopeClipboard = envData;
    MVAmpEnvelope::bEnvelopeClipboardEmpty = false;
    MVAmpEnvelope::envelopeClipboardMode = (displayMode == AMP ? 0 : 1);
}
void MVEnvelopeDrawer::pasteEnvelope()
{
    if( ! MVAmpEnvelope::bEnvelopeClipboardEmpty)
    {
        if(displayMode == MVAmpEnvelope::envelopeClipboardMode)
            for(int i=0;i<NB_ENV_PTS;i++)
                envData.points[i] = MVAmpEnvelope::envelopeClipboard.points[i];
        else
            for(int i=0;i<NB_ENV_PTS;i++)
                envData.points[i].frame = MVAmpEnvelope::envelopeClipboard.points[i].frame;

        envData.loop = MVAmpEnvelope::envelopeClipboard.loop;
        envData.sustainPoint = MVAmpEnvelope::envelopeClipboard.sustainPoint;
        envData.used = MVAmpEnvelope::envelopeClipboard.used;

        Globals::frame->updateUIs();
    }
}
