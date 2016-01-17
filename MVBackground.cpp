#include "MVBackground.h"

#include <QRadialGradient>
#include <QPainter>

void MVBackground::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.setBackground(*gradient);
    painter.eraseRect(0,0,width(),height());
//    painter.setPen(Qt::yellow);
//    painter.drawRect(0,0,width()-1,height()-1);
}

void MVBackground::setColors(QColor c1, QColor c2, QColor c3)
{
    gradient->setColorAt(0,c1);
    gradient->setColorAt(0.5,c2);
    gradient->setColorAt(1,c3);
}

QRadialGradient * MVBackground::gradient = new QRadialGradient(QPointF(100, 100), 200);
