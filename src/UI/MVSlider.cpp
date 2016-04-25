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

#include "MVSlider.h"

#include "MVCheckBoxAction.h"

#include <QAction>
#include <QPainter>
#include <QMouseEvent>

MVSlider::MVSlider(QWidget * parent, int def, QString name) : QSlider(parent), MVControllerWidget(name)
{
    setMinimumHeight(20);
    defaultValue = def;
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
    connect(midiLearnAction, SIGNAL(triggered()), this, SLOT(midiLearn()));
    connect(removeAssignAction, SIGNAL(triggered()), this, SLOT(removeAssign()));
    connect(cbInvertAction->getCheckBox(), SIGNAL(toggled(bool)), this, SLOT(invertMidiCC(bool)));
    connect(&timer,SIGNAL(timeout()),this,SLOT(startWaitingMessage()));
}
void MVSlider::mouseDoubleClickEvent(QMouseEvent *)
{
    setValue(defaultValue);
}

void MVSlider::mouseReleaseEvent(QMouseEvent *)
{
    // Do nothing for doubleclick to work -> set default value
}

void MVSlider::paintEvent(QPaintEvent *)
{
    int margin = 10;
    int lineWidth = 4;
    int ledR = 3;
    QPainter painter(this);
    painter.setBackground(Qt::black);
    painter.eraseRect(rect());

    QPen pen;
    pen.setColor(isEnabled() ? Qt::darkGreen : Qt::darkGray);
    pen.setWidth(lineWidth);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    QPointF c;

    if(orientation() == Qt::Horizontal)
    {
        c.rx() = margin+(width()-margin*2) * (value()-minimum()) / (maximum() - minimum());
        c.ry() = ledR;
        painter.drawLine(margin,ledR,width()-margin,ledR);
        if(isEnabled())
        {
            QString label = QString::number(2*(value()-50));
            QFont font = painter.font();
            font.setPointSize(ledR*2);
            painter.setFont(font);
            int w = painter.fontMetrics().width(label);
            int h = painter.fontMetrics().height();
            painter.setPen(Qt::yellow);
            w = painter.fontMetrics().width("R");
            painter.drawText(c.x()-w/2, c.y()+h, label);
            painter.drawText(0, 0, w, h, Qt::AlignLeft, tr("L"));
            painter.drawText(width()-w, 0, w, h, Qt::AlignRight, tr("R"));
        }
    }
    else
    {
        QString label = QString::number(value());
        int w = painter.fontMetrics().width(label);
        int h = painter.fontMetrics().height();

        c.rx() = width()/2;
        c.ry() = margin+(height()-margin*2) * (maximum()-value()) / (maximum() - minimum());
        painter.drawLine(width()/2, h , width()/2, height());
        if(isEnabled())
        {
            painter.setPen(Qt::yellow);
            painter.drawText(c.x()-w/2,c.y()-4,label);
        }
    }
    if(isEnabled())
    {
        QRadialGradient grad2(c.x()-2,c.y()-2, 12.0 );

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
                grad2.setColorAt(1, Qt::white);
                break;
        }
        painter.setPen(Qt::gray);
        painter.setBrush(grad2);
        painter.drawEllipse(c,ledR,ledR);
    }
}

void MVSlider::midiLearn()
{
    MVControllerWidget::midiLearn();
}
void MVSlider::removeAssign()
{
    MVControllerWidget::removeAssign();
}
void MVSlider::processCC(unsigned char v)
{
    if(isEnabled())
        setValue(minimum() +  v * ((float(maximum()-minimum())/127.0f)));
}
void MVSlider::startWaitingMessage()
{
    MVControllerWidget::startWaitingMessage();
}

void MVSlider::stopWaitingMessage()
{
    MVControllerWidget::stopWaitingMessage();
}
void MVSlider::contextMenu(QPoint)
{
    MVControllerWidget::displayMenu();
}
void MVSlider::invertMidiCC(bool b)
{
    MVControllerWidget::invertMidiCC(b);
}
