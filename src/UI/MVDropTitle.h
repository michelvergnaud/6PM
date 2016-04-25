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

#ifndef MVDROPTITLE_H
#define MVDROPTITLE_H

#include <QWidget>

class MVCheckBox;
class QLabel;


class MVDropTitle : public QWidget
{
    Q_OBJECT

public:
    MVDropTitle(QWidget *, QString, QWidget *parent, bool initialState);

    protected :
    void paintEvent(QPaintEvent *);

private slots:
    void checkBoxToggled(bool);

private:
    QLabel * label;
    MVCheckBox * checkBox;
    QWidget * dropWidget;
};

#endif // MVDROPTITLE_H
