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

#include "MVPushButton.h"

#include <QRadialGradient>
#include <QPainter>

MVPushButton::MVPushButton(QString  str, QWidget * parent) : QPushButton(parent)
{
    label = str;
    gradient = new QRadialGradient();
    bMouseOver = false;
    bPressed = false;
    ledOn = false;
}
MVPushButton::~MVPushButton()
{
    delete gradient;
}
void MVPushButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.setPen(bMouseOver && isEnabled() ? Qt::yellow : Qt::darkYellow);
    gradient->setCenter(width()/2,height()/2);
    gradient->setRadius(width());

    gradient->setColorAt(0, bMouseOver && isEnabled() ? color2 : color1);
    gradient->setColorAt(1, bMouseOver && isEnabled() ? color1 : color2);
    if(bPressed)
        p.setBrush(Qt::red);
    else
        p.setBrush(*gradient);

    if(! isEnabled())
        p.setPen( Qt::gray);
    p.drawRoundedRect(1,1,width()-2,height()-2,10,10);
    p.drawText(QRect(1,1,width()-2,height()-2), Qt::AlignCenter, label);

    if(ledOn)
    {
        p.setBrush(Qt::green);
        p.drawEllipse(QPoint(4*width()/5,height()/2),2,2);
    }
}
void MVPushButton::enterEvent(QEvent *)
{
    bMouseOver = true;
    update();
}
void MVPushButton::leaveEvent(QEvent *)
{
    bMouseOver = false;
    update();
}
void MVPushButton::mousePressEvent(QMouseEvent * e)
{
    QPushButton::mousePressEvent(e);
    bPressed = true;
    update();

}
void MVPushButton::mouseReleaseEvent(QMouseEvent * e)
{
    QPushButton::mouseReleaseEvent(e);
    bPressed = false;
    update();
}
void MVPushButton::showLed(bool b)
{
    ledOn = b;
    update();
}
QColor MVPushButton::color1;
QColor MVPushButton::color2;
