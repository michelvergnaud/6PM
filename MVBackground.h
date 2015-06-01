#ifndef MVBACKGROUND_H
#define MVBACKGROUND_H

#include <QWidget>

class QRadialGradient;

class MVBackground : public QWidget
{
public:
    static void setColors(QColor,QColor,QColor);

protected:

   void paintEvent(QPaintEvent *e);

private:
   static QRadialGradient * gradient;
};
#endif // MVBACKGROUND_H
