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

#ifndef MVDIAL_H
#define MVDIAL_H

#include "MVController.h"

#include <QDial>

class MVDial : public QDial, public MVController
{
    Q_OBJECT

public:
    MVDial(QWidget * parent, int def=0);
    void setLabel(QString s){label=s;}
    void setAssignable(bool b){bAssignable=b;}
    void setLabelColor(QColor c){labelColor=c;}
    void setMainColor(QColor);
    void updateWidget(){update();}

private slots:
    void contextMenu(QPoint);
    void midiLearn();
    void startWaitingMessage();
    void stopWaitingMessage();
    void removeAssign();
    void setParamValue(int);
    void invertMidiCC(bool);

protected:
    void paintEvent(QPaintEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent *);
private :
        int defaultValue;
        QString label;
        QColor labelColor, mainColor;
};

#endif // MVDIAL_H
