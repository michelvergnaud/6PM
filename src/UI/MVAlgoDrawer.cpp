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

#include "MVAlgoDrawer.h"

#include "Synth/MVNote.h"
#include "MVGlobals.h"
#include "MVMainFrame.h"

MVAlgoDrawer::MVAlgoDrawer()
{
    for(int i=0;i<NB_OSC;i++)
        graphicOscs[i] = new MVGraphicOsc(i);
    startConnectIn = -1;
    startConnectOut = -1;
    movingRect = -1;
}

MVAlgoDrawer::~MVAlgoDrawer()
{
    for(int i=0;i<NB_OSC;i++)
        delete graphicOscs[i];
}

QString MVAlgoDrawer::buildAlgoDescriptor()
{
    QString str;

    // Operators
    for(int i=0;i<NB_OSC;i++)
    {
        if(graphicOscs[i]->getPosY() == 1)
            str += QString::number(i+1);
    }
    str += " ";
    // Modulators
    for(int i=0;i<NB_OSC;i++)
    {
        for(int j=0;j<MVNote::modulatorsData[i].count();j++)
            str += QString("%1*%2 ").arg(i+1).arg(MVNote::modulatorsData[i].value(j)+1);
    }
    str = str.trimmed();
    return str;
}

void MVAlgoDrawer::drawConnection(QPainter & p, QPointF startPoint, QPointF endPoint)
{
    if((startPoint.y() > endPoint.y())
            || (startPoint.y() < endPoint.y() && startConnectIn > -1))
    {
        p.drawLine(startPoint,endPoint);
    }
    else
    {
        QPointF pts[6];
        pts[0] = startPoint;
        pts[5] = endPoint;

        int xStartOffset = startPoint.x() - endPoint.x() > stepX ? -1 : 1;
        int xEndOffset = startPoint.x() - endPoint.x() < -stepX ? -1 : 1;

        pts[1] = pts[0] + QPointF(0,-stepY/4);
        pts[2] = pts[1] + QPointF(xStartOffset*stepX/2,0);
        pts[3] = pts[5] + QPointF(xEndOffset*stepX/2,stepY/4);
        pts[4] = pts[5] + QPointF(0, stepY/4);

        p.drawPolyline(pts, 6);
    }
}

void MVAlgoDrawer::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    drawAlgo(p);
}

void MVAlgoDrawer::drawAlgo(QPainter & p)
{
    h = height();
    w = width();

    p.setBackground(Qt::black);
    p.eraseRect(rect());
    p.setBrush(Qt::yellow);

    QPen penRect;

    QPen penLines;
    penLines.setColor(Qt::yellow);
    penLines.setWidth(2);
    stepX = w/(NB_OSC+1);
    stepY= h/(NB_OSC+1);
    minPosX = 1;
    maxPosX = NB_OSC;
    minPosY = 1;
    maxPosY = NB_OSC;

    for(int i=0;i<NB_OSC;i++)
    {
        graphicOscs[i]->setPosX(i+1);
        graphicOscs[i]->setPosY(NB_OSC);
        graphicOscs[i]->setSize(QSize(w/8,h/8));
    }
    // Place operators
    switch (MVNote::nbOps)
    {
        case 1 : graphicOscs[MVNote::ops[0]]->setPosX(3);break;
        case 2 : graphicOscs[MVNote::ops[0]]->setPosX(2);graphicOscs[MVNote::ops[1]]->setPosX(4);break;
        case 3 : graphicOscs[MVNote::ops[0]]->setPosX(2);graphicOscs[MVNote::ops[1]]->setPosX(4);graphicOscs[MVNote::ops[2]]->setPosX(6);break;
        case 4 : for(int i=0;i<4;i++)graphicOscs[MVNote::ops[i]]->setPosX(i+2);break;
        case 5 : for(int i=0;i<5;i++)graphicOscs[MVNote::ops[i]]->setPosX(i+1);break;
        case 6 : for(int i=0;i<6;i++)graphicOscs[MVNote::ops[i]]->setPosX(i+1);break;
    }
    for(int i=0;i<MVNote::nbOps;i++)
        graphicOscs[MVNote::ops[i]]->setPosY(1);

    bool placed[NB_OSC];
    for(int i=0;i<NB_OSC;i++)
        placed[i] = false;
    // Place modulators
    for(int i=0;i<NB_OSC;i++)
    {
        int nbModulators = MVNote::modulatorsData[i].count();
        int offset = -nbModulators/2;
        for(int j=0;j<nbModulators;j++)
        {
            int n = MVNote::modulatorsData[i].at(j);
            if( ! MVNote::isOp(n))
            {
                if(n != i && ! placed[n])
                {
                    graphicOscs[n]->setPosX(graphicOscs[i]->getPosX() + offset);
                    graphicOscs[n]->setPosY(graphicOscs[i]->getPosY()+1);
                    placed[n] = true;
                    offset++;
                }
            }
        }
    }
    // Adjust size
    minPosX = NB_OSC;
    maxPosX = 1;
    minPosY = 1;
    maxPosY = 1;

    for(int i=0;i<NB_OSC;i++)
    {
        if((MVNote::isOp(graphicOscs[i]->getNum()) || MVNote::isModulated(graphicOscs[i]->getNum()) || MVNote::isModulator(graphicOscs[i]->getNum())))
        {
            minPosX = qMin(minPosX,graphicOscs[i]->getPosX());
            maxPosX = qMax(maxPosX,graphicOscs[i]->getPosX());
            maxPosY = qMax(maxPosY,graphicOscs[i]->getPosY());
        }
    }

    stepX = w/(maxPosX-minPosX+2);
    stepY = h/(maxPosY-minPosY+2);

    // Draw
    int yOps = stepY;

    for(int i=0;i<NB_OSC;i++)
    {
        if(i != movingRect)
            graphicOscs[i]->moveCenter(QPointF((graphicOscs[i]->getPosX()-minPosX+1)*stepX, h-yOps-(graphicOscs[i]->getPosY()-1)*stepY));
        p.setPen(penRect);
        if((MVNote::isOp(graphicOscs[i]->getNum()) ||
            MVNote::isModulated(graphicOscs[i]->getNum()) ||
            MVNote::isModulator(graphicOscs[i]->getNum()))  )
            graphicOscs[i]->draw(p);

        // Draw connections
        p.setPen(penLines);
        // Operators towards bottom
        if(MVNote::isOp(i))
        {
            QPointF pts[2];
            pts[0] = graphicOscs[i]->pOut();
            pts[1] = QPointF(pts[0].x(), h-yOps/4);
            p.drawPolyline(pts,2);
        }
    }
    if(MVNote::nbOps>1)
        p.drawLine(graphicOscs[MVNote::ops[0]]->center().x(),h-(yOps/4),graphicOscs[MVNote::ops[MVNote::nbOps-1]]->center().x(),h-(yOps/4));

    // Modulated towards modulators
    for(int i=0;i<NB_OSC;i++)
    {
        for(int j=0;j<MVNote::modulatorsData[i].count();j++)
        {
            int n = MVNote::modulatorsData[i].at(j);
            drawConnection(p, graphicOscs[i]->pIn(),graphicOscs[n]->pOut());

        }
    }
}
