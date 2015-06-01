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

#include "MVSlider.h"

#include "MVCheckBoxAction.h"

#include <QAction>
#include <QPainter>

MVSlider::MVSlider()
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
    connect(midiLearnAction, SIGNAL(triggered()), this, SLOT(midiLearn()));
    connect(removeAssignAction, SIGNAL(triggered()), this, SLOT(removeAssign()));
    connect(cbInvertAction->getCheckBox(), SIGNAL(toggled(bool)), this, SLOT(invertMidiCC(bool)));
    connect(&timer,SIGNAL(timeout()),this,SLOT(startWaitingMessage()));
}
void MVSlider::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    if( ! isEnabled())
        return;

    int margin = 4;
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::darkGreen);
    pen.setWidth(5);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    if(orientation() == Qt::Horizontal)
        painter.drawLine(margin,height()/2,width()-margin,height()/2);
    else
        painter.drawLine(width()/2,0, width()/2,height());

    QPointF c;
    if(orientation() == Qt::Horizontal)
    {
        c.rx() = margin+(width()-margin*2) * (value()-minimum()) / (maximum() - minimum());
        c.ry() = height()/2;
    }
    else
    {
        c.rx() = width()/2;
        c.ry() = margin+(height()-margin*2) * (maximum()-value()) / (maximum() - minimum());
    }

    QRadialGradient grad2(c.x()-2,c.y()-2, 12.0 );
    
    switch(state)
    {
        case MVController::NOASSIGN:
            grad2.setColorAt(0, Qt::white);
            grad2.setColorAt(1, Qt::gray);
            break;
        case MVController::ASSIGNED:
            grad2.setColorAt(0, Qt::cyan);
            grad2.setColorAt(1, Qt::blue);
            break;
        case MVController::WAITING:
            grad2.setColorAt(0, Qt::red);
            grad2.setColorAt(1, Qt::white);
            break;
    }
    painter.setPen(Qt::gray);
    painter.setBrush(grad2);
    painter.drawEllipse(c,6,6);
}

void MVSlider::midiLearn()
{
    MVController::midiLearn();
}
void MVSlider::removeAssign()
{
    MVController::removeAssign();
}
void MVSlider::setParamValue(int v)
{
    setValue(minimum() +  v * ((float(maximum()-minimum())/127.0f)));
}
void MVSlider::startWaitingMessage()
{
    MVController::startWaitingMessage();
}

void MVSlider::stopWaitingMessage()
{
    MVController::stopWaitingMessage();
}
void MVSlider::contextMenu(QPoint)
{
    MVController::displayMenu();
}
void MVSlider::invertMidiCC(bool b)
{
    MVController::invertMidiCC(b);
}
