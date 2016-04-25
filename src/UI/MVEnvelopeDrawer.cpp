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

#include "MVEnvelopeDrawer.h"

#include "MVSpinBoxAction.h"
#include "MVMainFrame.h"
#include "Synth/MVNote.h"

#include <QSpinBox>
#include <QPainter>
#include <QAction>
#include <QMenu>
#include <QLabel>
#include <QWheelEvent>
#include <math.h>

MVEnvelopeDrawer::MVEnvelopeDrawer(int & n) : numOsc(n)
{
    zoomX = 1.0;
    zoomY = 1.0;
    sustainPointAction = new MVSpinBoxAction(tr("Sustain point"));
    sustainPointAction->getSpinBox()->setMaximum(NB_ENV_PTS-1);
    sustainPointAction->getSpinBox()->setMinimum(0);
    sustainPointAction->getSpinBox()->setSpecialValueText(tr("None"));

    menu = new QMenu();
    menu->addAction(sustainPointAction);
    menu->addAction(tr("Copy"),this,SLOT(copyEnvelope()));
    pasteAction = new QAction(tr("Paste"), this);
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

void MVEnvelopeDrawer::preparePaint(QPainter & p)
{
    p.setBackground(Qt::black);
    p.eraseRect(0,0,width(),height());
    QFont font = p.font();
    font.setPointSizeF(6);
    p.setFont(font);

    // X axis
    int maxTime = framesTomsec(totalLength)/zoomX;
    float xStepMSec = 1.0;
    if(maxTime > 1000)
        xStepMSec = 100;
    else if(maxTime > 10)
        xStepMSec = 100;
    else if(maxTime > 10)
            xStepMSec = 1;

    int nbDigits;
    QString xLabel;
    if (maxTime > 1000)
    {
        nbDigits = 1;
        xLabel = "0.0";
    }
    else if (maxTime > 100)
    {
        nbDigits = 2;
        xLabel = "0.00";
    }
    else if (maxTime > 10)
    {
            nbDigits = 3;
            xLabel = "0.000";
    }
    else
    {
        nbDigits = 4;
        xLabel = "0.0000";
    }
    QRect rect = p.fontMetrics().boundingRect(xLabel);
    textW = rect.width();
    textH = rect.height();

    h = height()-2*textH-1;
    w = width()-textW-1;
    x = 0.0;
    y = 0.0;
    xSustainLine = 0;
    xMaxSustainLine = 0;

    float xStepPixel = zoomX * msecToFrames(xStepMSec) * w / totalLength;
    int factor = 2;
    while(xStepPixel/factor < textW * 2)
    {
        xStepMSec *= factor;
        xStepPixel *= factor;
        factor = (factor==5 ? 2 : 5);
    }
    factor = (factor==5 ? 2 : 5);
    while(xStepPixel/factor > textW * 2)
    {
        xStepMSec /= factor;
        xStepPixel /= factor;
        factor = (factor==5 ? 2 : 5);
    }

    float ax = 0;
    int i = 0;

    p.translate(textW*2,textH/2);
    while(ax < w)
    {
        ax = i*xStepPixel;
        p.setPen(QColor::fromRgb(32,32,32));
        p.drawLine(ax,0,ax,h);
        p.setPen(Qt::lightGray);
        QString label = "0";
        if(i>0) label = QString::number(i*xStepMSec/1000.0,'f',nbDigits);
        textW = p.fontMetrics().boundingRect(label).width();
        p.drawText(ax-textW/2, h+textH,label);
        i++;
    }
 }

void MVEnvelopeDrawer::wheelEvent(QWheelEvent * e)
{
    int z = e->angleDelta().y();
    if( e->modifiers() & Qt::ControlModifier )
    {
        if(z > 0 /*&& zoomX < 1000*/)
            zoomX *= 1.5;
        if(z<0 && zoomX > 1)
            zoomX *= 0.75;
        if(zoomX > 10000.0) zoomX = 10000.0;
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

MVEnvelopeDrawer::EnvelopeClipboard MVEnvelopeDrawer::envelopeClipboard;

