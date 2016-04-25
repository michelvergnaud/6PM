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

#include "MVController.h"

#include "MVGlobals.h"
#include "Synth/MVControlManager.h"
#include "Synth/MVFreqEnvelope.h"

MVController::MVController(QString s)
{
    name = s;
    bMidiCCInverted = false;
    state = NOASSIGN;
}
MVController::~MVController()
{
}
// --------------
MVControllerFloat::MVControllerFloat(QString name, float &v, float minv, float maxv)
    : MVController(name), value(v)
{
    minValue = minv;
    maxValue = maxv;
}
MVControllerFloat::~MVControllerFloat()
{
}
void MVControllerFloat::processCC(unsigned char param)
{
    value = minValue + (maxValue-minValue) * (float)param / 127.0f;
}

// --------------
MVControllerInt::MVControllerInt(QString name, int & v, int minv, int maxv) : MVController(name), value(v)
{
    minValue = minv;
    maxValue = maxv;
}
MVControllerInt::~MVControllerInt()
{
}
void MVControllerInt::processCC(unsigned char param)
{
    value = minValue + (maxValue-minValue) * (float)param / 127.0;
}

// --------------
MVControllerBool::MVControllerBool(QString name, bool &v) : MVController(name), value(v)
{
}
MVControllerBool::~MVControllerBool()
{
}
void MVControllerBool::processCC(unsigned char param)
{
    value = param > 63;
}

// --------------
MVControllerEnvFrame::MVControllerEnvFrame(QString name, unsigned long & v) : MVController(name), value(v)
{
}
MVControllerEnvFrame::~MVControllerEnvFrame()
{
}
void MVControllerEnvFrame::processCC(unsigned char param)
{
    int i = MVFreqEnvelope::nbTimeValues * (float)param / 127.0f;
    value = MVFreqEnvelope::timeValues[i];
}

// --------------
MVControllerFreqRatio::MVControllerFreqRatio(QString name, float & v, float minv, float maxv) : MVControllerFloat(name, v, minv, maxv)
{
}
MVControllerFreqRatio::~MVControllerFreqRatio()
{
}
void MVControllerFreqRatio::processCC(unsigned char param)
{
    float v = minValue + (maxValue-minValue) * (float)param / 127.0f;
    value = pow(TWELFTH_ROOT2, v);
}

// --------------
MVControllerDetune::MVControllerDetune(QString name, float & v, float minv, float maxv) : MVControllerFloat(name, v, minv, maxv)
{
}
MVControllerDetune::~MVControllerDetune()
{
}
void MVControllerDetune::processCC(unsigned char param)
{
    float v = minValue + (maxValue-minValue) * (float)param / 127.0f;
    value = pow(TWELFTH_ROOT2, v/100.0f);
}

// --------------
MVControllerFreqValue::MVControllerFreqValue(QString name, float & v, float minv, float maxv) : MVControllerFloat(name, v, minv, maxv)
{
}
MVControllerFreqValue::~MVControllerFreqValue()
{
}
void MVControllerFreqValue::processCC(unsigned char param)
{
    float v = minValue + (maxValue-minValue) * (float)param / 127.0f;
    value = pow(TWELFTH_ROOT2, v);
}
