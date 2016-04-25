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

#ifndef MVCONTROLLER_H
#define MVCONTROLLER_H

#include <QString>

class MVController
{
public:
    MVController(QString);

    enum ControllerState {NOASSIGN,ASSIGNED,WAITING};

    QString getName() const {return name;}
    void setName(QString n) {name = n;}
    ControllerState getState() const {return state;}
    void setState(ControllerState s){state = s;}
    bool isMidiCCInverted(){return bMidiCCInverted;}
    void invertMidiCC(bool b){bMidiCCInverted = b;}

    virtual void processCC(unsigned char) = 0;

protected :
    virtual ~MVController();
    ControllerState state;
    bool bMidiCCInverted;

private:
    QString name;
};

// --------------
class MVControllerFloat : public MVController
{
public:
    MVControllerFloat(QString, float &, float,float);
    virtual void processCC(unsigned char);
protected :
    virtual ~MVControllerFloat();

    float & value;
    float minValue;
    float maxValue;
};

// --------------
class MVControllerInt : public MVController
{
public:
    MVControllerInt(QString, int &, int, int);
    virtual void processCC(unsigned char);
protected :
    virtual ~MVControllerInt();
    int & value;
    int minValue;
    int maxValue;
};

// --------------
class MVControllerBool : public MVController
{
public:
    MVControllerBool(QString, bool &);
    virtual void processCC(unsigned char);
protected :
    virtual ~MVControllerBool();
    bool & value;
};

// --------------
class MVControllerEnvFrame : public MVController
{
public:
    MVControllerEnvFrame(QString, unsigned long &);
    virtual void processCC(unsigned char);
protected :
    virtual ~MVControllerEnvFrame();
private:
    unsigned long & value;
};

// --------------
class MVControllerFreqRatio : public MVControllerFloat
{
public:
    MVControllerFreqRatio(QString, float &, float minv, float maxv);
    virtual void processCC(unsigned char);
protected :
    virtual ~MVControllerFreqRatio();
};

// --------------
class MVControllerDetune : public MVControllerFloat
{
public:
    MVControllerDetune(QString, float &, float minv, float maxv);
    virtual void processCC(unsigned char);
protected :
    virtual ~MVControllerDetune();
};
// --------------
class MVControllerFreqValue : public MVControllerFloat
{
public:
    MVControllerFreqValue(QString, float &, float minv, float maxv);
    virtual void processCC(unsigned char);
protected :
    virtual ~MVControllerFreqValue();
};

#endif // MVCONTROLLER_H
