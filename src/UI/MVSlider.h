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

#ifndef MVSLIDER_H
#define MVSLIDER_H

#include "MVControllerWidget.h"

#include <QSlider>

class QAction;

class MVSlider : public QSlider, public MVControllerWidget
{
    Q_OBJECT

public:
    MVSlider(QWidget * parent, int def, QString name);

private slots:
    void contextMenu(QPoint);
    void midiLearn();
    void startWaitingMessage();
    void stopWaitingMessage();
    void removeAssign();
    void processCC(unsigned char);
    void updateWidget(){update();}
    void invertMidiCC(bool);
protected:
    void paintEvent(QPaintEvent *);
    void mouseDoubleClickEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent *);

 };

#endif // MVSLIDER_H
