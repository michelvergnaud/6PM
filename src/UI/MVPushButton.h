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

#ifndef MVPUSHBUTTON_H
#define MVPUSHBUTTON_H

#include <QPushButton>

class QRadialGradient;

class MVPushButton : public QPushButton
{
public:
    MVPushButton(QString , QWidget*);
    ~MVPushButton();

    void showLed(bool);

    static void setColors(QColor c1, QColor c2){color1=c1;color2=c2;}

protected:
    void paintEvent(QPaintEvent*);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent * );

private:
    QString label;
    bool bMouseOver;
    bool bPressed;
    QRadialGradient * gradient;
    bool ledOn;
    static QColor color1;
    static QColor color2;
};

#endif // MVPUSHBUTTON_H
