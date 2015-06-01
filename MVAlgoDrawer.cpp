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

#include "MVAlgoDrawer.h"

#include "MVNote.h"
#include "MVGlobals.h"
#include "MVFrame.h"

#include <QMouseEvent>
#include <QMenu>

//----------------------------------------------------------
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
        QPen pen = p.pen();
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
    static QSize getRectSize(){return rectSize;}

private:
    int num;
    int posX;
    int posY;
    static QSize rectSize;
};
QSize MVGraphicOsc::rectSize = QSize(30,40);

//----------------------------------------------------------
MVAlgoDrawer::MVAlgoDrawer()
{
    //numAlgo = 0;
    bEditing = false;
    for(int i=0;i<NB_OSC;i++)
        graphicOscs[i] = new MVGraphicOsc(i);
    bFirst = false;
    startConnectIn = -1;
    startConnectOut = -1;
    movingRect = -1;
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}

MVAlgoDrawer::~MVAlgoDrawer()
{
    for(int i=0;i<NB_OSC;i++)
        delete graphicOscs[i];
}
void MVAlgoDrawer::startEdit()
{
    saveDescriptor = getAlgoDescriptor();
    bEditing = true;
    update();
}

void MVAlgoDrawer::endEdit(bool restoreSaved)
{
    if(restoreSaved)
        MVNote::createAlgo(saveDescriptor);
    bEditing = false;
    update();
}

QString MVAlgoDrawer::getAlgoDescriptor()
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
        str += " ";
    }
    return str;
}

void MVAlgoDrawer::contextMenu(QPoint p)
{
    for(int i=0;i<NB_OSC;i++)
    {
        if(graphicOscs[i]->contains(p))
        {
            std::cout << i<< std::endl;
            QMenu * menu = new QMenu(this);
            menu->addAction(QString(" Oscillator %1").arg(i+1));
            menu->addSeparator();

            QList<int> list = MVNote::modulatorsData[i];

            for(int j=0; j<list.count();j++)
            {
                QAction * action = new QAction(QString("Remove modulator %1").arg(list.at(j)+1),this);
                action->setData(i*NB_OSC+j); // Surely a better way to pass data to slot...
                connect(action,SIGNAL(triggered()),this,SLOT(removeModulator()));
                menu->addAction(action);
            }
            menu->popup(QCursor::pos());
        }
    }
}

void MVAlgoDrawer::removeModulator()
{
    int i = ((QAction*)sender())->data().toInt()/NB_OSC;
    int j = ((QAction*)sender())->data().toInt()%NB_OSC;
    MVNote::modulatorsData[i].removeAt(j);
    MVNote::createAlgo(getAlgoDescriptor());
    update();
}

void MVAlgoDrawer::mousePressEvent(QMouseEvent * e)
{
    if(!bEditing)
        return;

    movingRect = -1;

    int i=0;
    while(i<NB_OSC && movingRect == -1)
    {
        if(graphicOscs[i]->isOnPin(e->pos()))
            startConnectIn = i;
        else
        if(graphicOscs[i]->isOnPout(e->pos()))
            startConnectOut = i;
        else
        if(graphicOscs[i]->contains(e->pos()))
            movingRect = i;
        i++;
    }
}

void MVAlgoDrawer::mouseMoveEvent(QMouseEvent * e)
{
    if(!bEditing)
        return;

    QPoint p = e->pos();

    if(startConnectIn > -1 || startConnectOut > -1)
    {
        endDrawingLine = e->pos();
        update();
    }

    if(movingRect>-1)
    {
        graphicOscs[movingRect]->moveCenter(p);
        update();
    }
}
void MVAlgoDrawer::mouseReleaseEvent(QMouseEvent * e)
{
    if(!bEditing)
        return;

    for(int i=0;i<NB_OSC;i++)
    {
        if(graphicOscs[i]->isOnPin(e->pos()) && startConnectOut > -1)
        {
            if( ! MVNote::modulatorsData[i].contains(startConnectOut))
            {
                MVNote::modulatorsData[i].append(startConnectOut);
                qSort(MVNote::modulatorsData[i]);
            }
        }
        else
        if(graphicOscs[i]->isOnPout(e->pos()) && startConnectIn > -1)
        {
            if( ! MVNote::modulatorsData[startConnectIn].contains(i))
            {
                MVNote::modulatorsData[startConnectIn].append(i);
                qSort(MVNote::modulatorsData[startConnectIn]);
            }
        }
    }

    if(movingRect > -1)
    {
        int px = (e->x()+stepX/2)/stepX;
        int py = (height()- e->y()+stepY/2)/stepY;
        if(px<1)px=1;
        if(py<1)py=1;
        if(px>NB_OSC)px=NB_OSC;
        if(py>NB_OSC)py=NB_OSC;
        graphicOscs[movingRect]->setPosX(px);
        graphicOscs[movingRect]->setPosY(py);

        QPointF p = QPointF(px*stepX,height()-py*stepY);
        graphicOscs[movingRect]->moveCenter(p);
    }
    MVNote::createAlgo(getAlgoDescriptor());
    startConnectOut = -1;
    startConnectIn = -1;
    movingRect = -1;
    Globals::frame->updateUIs();
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
    int h = height();
    int w = width();

    p.setBackground(Qt::black);
    p.eraseRect(rect());
    p.setBrush(Qt::yellow);

    QPen penRect;

    QPen penLines;
    penLines.setColor(Qt::yellow);
    penLines.setWidth(2);
    stepX = w/(NB_OSC+1);
    stepY= h/(NB_OSC+1);
    int minPosX = 1;
    int maxPosX = NB_OSC;
    int minPosY = 1;
    int maxPosY = NB_OSC;

    if(bEditing)
    {
        for(int i=0;i<NB_OSC;i++)
        {
            if(bFirst)
            {
                graphicOscs[i]->setPosX(i+1);
                graphicOscs[i]->setPosY(NB_OSC);
                MVNote::modulatorsData[i].clear();
                MVNote::createAlgo(getAlgoDescriptor());
            }
        }
        bFirst = false;

        // Drawing connection
        p.setPen(penLines);
        if(startConnectIn > -1)
            drawConnection(p, endDrawingLine, graphicOscs[startConnectIn]->pIn());
        if(startConnectOut > -1)
            drawConnection(p, endDrawingLine, graphicOscs[startConnectOut]->pOut());
    }
    else
    {
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

        if(!bEditing)
        {
            // Place MVNote::modulatorsData
            for(int i=0;i<NB_OSC;i++)
            {
                int nbModulators = MVNote::modulatorsData[i].count();
                int offset = -nbModulators/2;
                for(int j=0;j<nbModulators;j++)
                {
                    int n = MVNote::modulatorsData[i].at(j);
                    if( ! MVNote::isOp(n))
                    {
                        if(n != i) // Self modulated, don't draw twice
                        {
                            graphicOscs[n]->setPosX(graphicOscs[i]->getPosX() + offset);
                            graphicOscs[n]->setPosY(graphicOscs[i]->getPosY()+1);
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
                if(bEditing || (MVNote::isOp(graphicOscs[i]->getNum()) || MVNote::isModulated(graphicOscs[i]->getNum()) || MVNote::isModulator(graphicOscs[i]->getNum())))
                {
                    minPosX = qMin(minPosX,graphicOscs[i]->getPosX());
                    maxPosX = qMax(maxPosX,graphicOscs[i]->getPosX());
                    maxPosY = qMax(maxPosY,graphicOscs[i]->getPosY());
                }
            }

            stepX = w/(maxPosX-minPosX+2);
            stepY = h/(maxPosY-minPosY+2);
        }
    } // ! bEditing

    // Draw
    int yOps = stepY;
    if(bEditing)
    {
        // Draw grid
        p.setPen(Qt::darkGray);
        for(int i=minPosX;i<=maxPosX;i++)
            p.drawLine(i*stepX, 0, i*stepX,h);
        for(int i=1;i<=maxPosY;i++)
            p.drawLine(0,h-i*stepY, w, h-i*stepY);
        // Operators line
        p.setPen(Qt::green);
        p.drawLine(0,h-yOps,w,h-yOps);
    }
    for(int i=0;i<NB_OSC;i++)
    {
        if(i != movingRect)
            graphicOscs[i]->moveCenter(QPointF((graphicOscs[i]->getPosX()-minPosX+1)*stepX, h-yOps-(graphicOscs[i]->getPosY()-1)*stepY));
        p.setPen(penRect);
        if(bEditing || (MVNote::isOp(graphicOscs[i]->getNum()) || MVNote::isModulated(graphicOscs[i]->getNum()) || MVNote::isModulator(graphicOscs[i]->getNum())))
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
