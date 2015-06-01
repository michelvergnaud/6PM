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

#ifndef MVALGODRAWER_H
#define MVALGODRAWER_H

#include "MVDefines.h"

#include <QWidget>
#include <QPainter>
#include <QList>

class MVGraphicOsc;

class MVAlgoDrawer : public QWidget
{
    Q_OBJECT
public:
    MVAlgoDrawer();
    ~MVAlgoDrawer();

    QString getAlgoDescriptor();
    bool isEditing(){return bEditing;}

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

public slots:
    void startEdit();
    void endEdit(bool);

private slots:
    void contextMenu(QPoint);
    void removeModulator();

private:
    bool bFirst;
    QString saveDescriptor;
    bool bEditing;
    int startConnectIn;
    int startConnectOut;
    QPoint endDrawingLine;
    void drawConnection(QPainter&,QPointF, QPointF);

    int movingRect;
    float stepX, stepY;
    MVGraphicOsc * graphicOscs[NB_OSC];
};

#endif // MVALGODRAWER_H
