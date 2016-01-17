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

#ifndef MVENVELOPPEDRAWER_H
#define MVENVELOPPEDRAWER_H

#include "MVAmpEnvelope.h"

#include <QWidget>
#include <QPainter>

class QAction;
class MVSpinBoxAction;
class QMenu;

class MVEnvelopeDrawer : public QWidget
{
Q_OBJECT

public:
    enum DisplayMode{AMP,FREQ};

    MVEnvelopeDrawer(MVAmpEnvelope::EnvelopeData &, int &, DisplayMode);
    ~MVEnvelopeDrawer();

    void setFill(bool);
    void setY0pos(float);
    void setLogDisplay(bool);
    void setTimeScale(float, float);


private slots:
    void contextMenu(QPoint);
    void sustainPointChanged(int);
    void copyEnvelope();
    void pasteEnvelope();

protected:
    void paintEvent ( QPaintEvent *) ;
    void wheelEvent(QWheelEvent *);

private:

    int textW;
    int textH;
    float h, w, x, y;
    QPointF pts[NB_ENV_PTS+3];
    float xSustainLine, xMaxSustainLine;
    void drawFreq(QPainter&);
    void drawAmp(QPainter&);
    DisplayMode displayMode;
    float totalLength;
    float maxSustainPointTime;

    MVAmpEnvelope::EnvelopeData & envData;
    const int & numOsc;
    float zoomX, zoomY;
    QMenu * menu;
    QAction * pasteAction;
    MVSpinBoxAction * sustainPointAction;
};

#endif // MVENVELOPPEDRAWER_H
