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
