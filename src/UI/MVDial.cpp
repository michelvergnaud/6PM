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

#include "MVDial.h"

#include "Synth/MVControlManager.h"
#include "MVCheckBoxAction.h"

#include <QPainter>
#include <QAction>
#include <QPixmap>

MVDial::MVDial(QWidget * parent, int def, QString name) : QDial(parent), MVControllerWidget(name)
{
    defaultValue = def;
    setMinimumSize(20,20);
    setPageStep(10);
    setSingleStep(1);
    labelColor = Qt::yellow;
    mainColor = Qt::black;
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
    connect(midiLearnAction, SIGNAL(triggered()), this, SLOT(midiLearn()));
    connect(removeAssignAction, SIGNAL(triggered()), this, SLOT(removeAssign()));
    connect(cbInvertAction->getCheckBox(), SIGNAL(toggled(bool)), this, SLOT(invertMidiCC(bool)));
    connect(&timer,SIGNAL(timeout()),this,SLOT(startWaitingMessage()));
}
void MVDial::setMainColor(QColor c)
{
    mainColor = c;
    update();
}

void MVDial::mouseDoubleClickEvent(QMouseEvent *)
{
    setValue(defaultValue);
}

void MVDial::mouseReleaseEvent(QMouseEvent *)
{
    // Do nothing for doubleclick to work -> set default value
}

void MVDial::paintEvent(QPaintEvent *e)
{
    if( ! isEnabled())
    {
        QDial::paintEvent(e);
        return;
    }

    QPainter painter(this);

    qreal r;
    if (width()>height())
        r = height()/2;
    else
        r = width()/2;

    qreal xc = width() * 0.5;
    qreal yc = height() * 0.5;
    painter.save();

    painter.translate(xc, yc);

    double a = 270 * (value()-minimum()) / (maximum()-minimum());
    painter.save();
    painter.rotate(a);
    QConicalGradient gradient(QPointF(0, 0), 225.0);
    gradient.setColorAt(0, mainColor);
    gradient.setColorAt(1, QColor::fromRgb(80,80,80));
    gradient.setSpread(QGradient::RepeatSpread);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setBrush(gradient);

    QPen pen;
    pen.setColor(Qt::lightGray);
    painter.setPen(pen);
    painter.drawEllipse(QPointF(0,0),r,r);

    QRadialGradient grad2(-r/2,r/2,r/4);
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
            grad2.setColorAt(1, Qt::gray);
            break;
    }
    painter.setBrush(grad2);
    painter.drawEllipse(QPointF(-r/2,r/2),r/8,r/8);

    painter.restore();
    if(isEnabled())
    {
        if(pixmaps.size() > 0 && pixmaps.size() > value())
        {
            painter.restore();
            painter.drawPixmap(r/2,r/2,r,r,*pixmaps.at(value()));
        }
        else
        {
            QFont font = painter.font();
            font.setPointSize(r/3);
            font.setBold(true);
            painter.setFont(font);
            int h = painter.fontMetrics().height();
            QRect rect(-r,-h,2*r,2*h);
            painter.setPen(labelColor);
            painter.drawText(rect, Qt::AlignCenter, label);
            painter.restore();
        }
    }
}
void MVDial::midiLearn()
{
    MVControllerWidget::midiLearn();
}

void MVDial::removeAssign()
{
    MVControllerWidget::removeAssign();
}

void MVDial::processCC(unsigned char v)
{
    if(isEnabled())
        setValue(minimum() +  v * ((float(maximum()-minimum())/127.0f)));
}

void MVDial::startWaitingMessage()
{
    MVControllerWidget::startWaitingMessage();
}

void MVDial::stopWaitingMessage()
{
    MVControllerWidget::stopWaitingMessage();
}

void MVDial::contextMenu(QPoint)
{
    MVControllerWidget::displayMenu();
}
void MVDial::invertMidiCC(bool b)
{
    MVControllerWidget::invertMidiCC(b);
}
