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

#ifndef MVENVELOPPEDRAWER_H
#define MVENVELOPPEDRAWER_H

#include "Synth/MVAmpEnvelope.h"

#include <QWidget>
#include <QPainter>

class QAction;
class MVSpinBoxAction;
class QMenu;
class MVEnvelopeDrawer : public QWidget
{
Q_OBJECT

public:
    struct EnvelopeClipboard
    {
      int typeEnv; //-1: nothing 0:AMP 1:FREQ
      AmpEnvelopeData ampData;
      FreqEnvelopeData freqData;
    };

    MVEnvelopeDrawer(int &);
    ~MVEnvelopeDrawer();

    void setFill(bool);
    void setY0pos(float);
    void setLogDisplay(bool);
    void setTimeScale(float, float);
    static EnvelopeClipboard envelopeClipboard;

private slots:
    virtual void contextMenu(QPoint) = 0;
    virtual void sustainPointChanged(int) = 0;
    virtual void copyEnvelope() = 0;
    virtual void pasteEnvelope() = 0;

protected:
    void preparePaint(QPainter &);
    void wheelEvent(QWheelEvent *);

    int textW;
    int textH;
    float h, w, x, y;
    QPointF pts[NB_ENV_PTS+3];
    float xSustainLine, xMaxSustainLine;

    float totalLength;
    float maxSustainPointTime;

    const int & numOsc;
    float zoomX, zoomY;
    QMenu * menu;
    QAction * pasteAction;
    MVSpinBoxAction * sustainPointAction;
};

#endif // MVENVELOPPEDRAWER_H
