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

#ifndef MVALGODRAWER_H
#define MVALGODRAWER_H

#include "MVDefines.h"

#include <QWidget>
#include <QPainter>
#include <QList>

class MVGraphicOsc : public QRectF
{
public:
    MVGraphicOsc(int n) : QRectF()
    {
        num = n;
        setSize(QSize(30,40));
        posX = 0;
        posY = 0;
    }
    int getNum(){return num;}
    int getPosX(){return posX;}
    int getPosY(){return posY;}
    void setPosX(int p){if (p>0 && p<=NB_OSC) posX=p;}
    void setPosY(int p){if (p>0 && p<=NB_OSC) posY=p;}
    bool isOnPin(QPointF p){return QPointF(pIn() - p).manhattanLength() < 10; }
    bool isOnPout(QPointF p){return QPointF(pOut() - p).manhattanLength() < 10; }

    void draw(QPainter & p)
    {
        // Shadow
        QRectF shadowRect = *this;
        shadowRect.translate(3,3);
        QBrush br = p.brush();
        p.setPen(Qt::black);
        p.setBrush(Qt::darkGray);
        p.drawRect(shadowRect);
        p.setBrush(br);
        p.drawRect(*this);
        p.drawText(*this,Qt::AlignCenter, QString::number(num+1));
        p.drawEllipse(pIn(), 2,2);
        p.drawEllipse(pOut(), 2,2);
    }
    QPointF pIn(){return (topRight()+topLeft())/2;}
    QPointF pOut(){return (bottomRight()+bottomLeft())/2;}

private:
    int num;
    int posX;
    int posY;
};

class MVAlgoDrawer : public QWidget
{
public:
    MVAlgoDrawer();
    virtual ~MVAlgoDrawer();
    QString buildAlgoDescriptor();

protected:
    void paintEvent(QPaintEvent *);
    virtual void drawAlgo(QPainter &);
    QString saveDescriptor;
    int startConnectIn;
    int startConnectOut;
    QPoint endDrawingLine;
    void drawConnection(QPainter&,QPointF, QPointF);
    int movingRect;
    float stepX, stepY;
    int h,w, minPosX, maxPosX, minPosY, maxPosY;
    MVGraphicOsc * graphicOscs[NB_OSC];
};

#endif // MVALGODRAWER_H
