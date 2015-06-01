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

#include "MVMeter.h"

#include <QPainter>
#include <QLinearGradient>
#include <QTimer>

#include <math.h>

MVMeter::MVMeter(float *buf, int n)
{
    buffer = buf;
    bufferSize = n;
    timer = new QTimer();
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start();
}
MVMeter::~MVMeter()
{
    delete timer;
}
void MVMeter::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setBackground(Qt::black);
    p.setBrush(Qt::green);
    float h = height();
    float w = width();
    p.eraseRect(0,0,w,h);

    float amp=0;
    for( int i=0;i<bufferSize;i++)
        amp = qMax((float)fabs(buffer[i]),amp);

    QRectF rect(0,h,w,-h*amp);
    QLinearGradient gradient(0,h,w,0);
    gradient.setColorAt(0, Qt::green);
    gradient.setColorAt(0.5, Qt::yellow);
    gradient.setColorAt(1, Qt::red);
    p.fillRect(rect, gradient);
}
