#include "MVCheckBox.h"

#include "MVCheckBoxAction.h"

#include<QPainter>

MVCheckBox::MVCheckBox(QString strOn, QString strOff) : QCheckBox()
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
   int r = qMin(width(),height()) / 2;
   painter.drawPixmap( width()/2-r, height()/2-r, r*2, r*2, (isChecked() ? *pixmapOn : *pixmapOff) );
   QPen pen;
   pen.setColor(Qt::black);
   painter.setPen(pen);
   QRadialGradient grad2(QPointF(width()/2,height()/2),5);

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
           grad2.setColorAt(1, QColor::fromRgb(255,128,128));
           break;
   }
   painter.setBrush(grad2);
   painter.drawEllipse(QPointF(width()/2,height()/2),2,2);
}
void MVCheckBox::midiLearn()
{
    MVController::midiLearn();
}
void MVCheckBox::removeAssign()
{
 MVController::removeAssign();
}
void MVCheckBox::setParamValue(int v)
{
    setChecked(v>63);
}
void MVCheckBox::startWaitingMessage()
{
    MVController::startWaitingMessage();
}
void MVCheckBox::stopWaitingMessage()
{
    MVController::stopWaitingMessage();
}
void MVCheckBox::contextMenu(QPoint)
{
    MVController::displayMenu();
}
void MVCheckBox::invertMidiCC(bool b)
{
    MVController::invertMidiCC(b);
}
