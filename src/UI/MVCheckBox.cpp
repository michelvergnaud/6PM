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

#include "MVCheckBox.h"

#include "MVCheckBoxAction.h"

#include<QPainter>

MVCheckBox::MVCheckBox(QWidget * parent, QString strOn, QString strOff, QString name) : QCheckBox(parent),MVControllerWidget(name)
{
    pixmapOn = new QPixmap(strOn);
    pixmapOff = new QPixmap(strOff);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
    connect(midiLearnAction, SIGNAL(triggered()), this, SLOT(midiLearn()));
    connect(removeAssignAction, SIGNAL(triggered()), this, SLOT(removeAssign()));
    connect(cbInvertAction->getCheckBox(), SIGNAL(toggled(bool)), this, SLOT(invertMidiCC(bool)));
    connect(&timer,SIGNAL(timeout()),this,SLOT(startWaitingMessage()));
}

MVCheckBox::~MVCheckBox()
{
    delete pixmapOff;
    delete pixmapOn;
}

bool MVCheckBox::hitButton(const QPoint & pos) const
{
    return rect().contains(pos);
}

void MVCheckBox::paintEvent(QPaintEvent *)
{
   QPainter painter(this);
   QPen pen;
   pen.setColor(Qt::black);
   painter.setPen(pen);

   int r = qMin(width(),height()) / 2;

   if( ! isEnabled())
   {
      painter.drawEllipse(QPointF(width()/2,height()/2),r,r);
      return;
   }

   painter.drawPixmap( width()/2-r, height()/2-r, r*2, r*2, (isChecked() ? *pixmapOn : *pixmapOff) );

    if(bAssignable)
    {
       QRadialGradient grad2(QPointF(width()/2,height()/2),5);

       switch(state)
       {
           case MVControllerWidget::NOASSIGN:
               grad2.setColorAt(0, Qt::white);
               grad2.setColorAt(1, Qt::gray);
               break;
           case MVControllerWidget::ASSIGNED:
               grad2.setColorAt(0, Qt::cyan);
               grad2.setColorAt(1, Qt::blue);
               break;
           case MVControllerWidget::WAITING:
               grad2.setColorAt(0, Qt::red);
               grad2.setColorAt(1, QColor::fromRgb(255,128,128));
               break;
       }

       painter.setBrush(grad2);
       painter.drawEllipse(QPointF(width()/2,height()/2),2,2);
   }
}
void MVCheckBox::midiLearn()
{
    MVControllerWidget::midiLearn();
}
void MVCheckBox::removeAssign()
{
 MVControllerWidget::removeAssign();
}
void MVCheckBox::processCC(unsigned char v)
{
    if(isEnabled())
        setChecked(v>63);
}
void MVCheckBox::startWaitingMessage()
{
    MVControllerWidget::startWaitingMessage();
}
void MVCheckBox::stopWaitingMessage()
{
    MVControllerWidget::stopWaitingMessage();
}
void MVCheckBox::contextMenu(QPoint)
{
    MVControllerWidget::displayMenu();
}
void MVCheckBox::invertMidiCC(bool b)
{
    MVControllerWidget::invertMidiCC(b);
}
