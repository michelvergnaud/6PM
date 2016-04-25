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

#ifndef MVPRESET_H
#define MVPRESET_H


#include "MVDefines.h"

#include <QStringList>


struct OscillatorData
{
    float pan;
    float amp;
    float freqratio;
    float detune;
    float iMod;
    float instantFreq;
    float randomize;
};

struct LfoData
{
    float amp;
    float freq;
    int shape;
};

struct EnvelopePoint
{
    unsigned long frame;
    float value;
};

struct AmpEnvelopeData
{
    EnvelopePoint points[NB_ENV_PTS];
    int sustainPoint;
    bool loop;
};

struct FreqEnvelopeData
{
    EnvelopePoint points[NB_ENV_PTS];
    int sustainPoint;
    bool loop;
    bool used;
    bool absolute;
};

struct FilterData
{
    bool used;
    int type;
    float fRatio;
    float q;
};


class MVPreset
{
public:
    MVPreset();
    MVPreset(QString);

    bool rename(QString);
    bool readData();
    bool saveData();
    bool saveData(QString);
    void setModified(bool);
    bool isModified(){return bModified;}
    QString getName() {return name;}
    QString getFileName() {return fileName;}
    void copyDataFrom(MVPreset & other);

    OscillatorData * getOscData(int i) {return &oscData[i];}
    LfoData * getTremData(int i){return &tremData[i];}
    LfoData * getVibData(int i){return &vibData[i];}
    AmpEnvelopeData * getOscAmpEnvData(int i) {return &oscAmpEnvData[i];}
    FreqEnvelopeData * getOscFreqEnvData(int i) {return &oscFreqEnvData[i];}
    FilterData * getFilterData() {return &filterData;}
    FreqEnvelopeData * getFilterEnvData() {return &filterEnvData;}

    void setPredefAlgoIndex(int);
    void setUserAlgoDescriptor(QString);
    bool isPreDefAlgo(){return bPredefAlgo;}
    void setPredefAlgo(bool b){bPredefAlgo = b;}
    int getPredefAlgoIndex(){return predefAlgoIndex;}
    QString getUserAlgoDescriptor(){return QString(userAlgoDescriptor);}

private:
    QString name;
    QString fileName;
    OscillatorData oscData[NB_OSC];
    LfoData tremData[NB_OSC];
    LfoData vibData[NB_OSC];
    AmpEnvelopeData oscAmpEnvData[NB_OSC];
    FreqEnvelopeData oscFreqEnvData[NB_OSC];
    FilterData filterData;
    FreqEnvelopeData filterEnvData;

    bool bPredefAlgo;
    int predefAlgoIndex;
    QString userAlgoDescriptor;
    static OscillatorData defaultOscData[NB_OSC];
    static LfoData defaultTremData[NB_OSC];
    static LfoData defaultVibData[NB_OSC];
    static AmpEnvelopeData defaultOscAmpEnvData[NB_OSC];
    static FreqEnvelopeData defaultOscFreqEnvData[NB_OSC];
    static FilterData defaultFilterData;
    static FreqEnvelopeData defaultFilterEnvData;

    bool bModified;

    static QString PRST_TAG_PreDefinedAlgo;
    static QString PRST_TAG_numAlgo;
    static QString PRST_TAG_algoDescriptor;
    static QString PRST_TAG_osc1pan;
    static QString PRST_TAG_osc1amp;
    static QString PRST_TAG_osc1freqratio;
    static QString PRST_TAG_osc1detune;
    static QString PRST_TAG_osc1random;
    static QString PRST_TAG_osc1iMod;
    static QString PRST_TAG_osc1tremAmp;
    static QString PRST_TAG_osc1tremFreq;
    static QString PRST_TAG_osc1tremShape;
    static QString PRST_TAG_osc1vibAmp;
    static QString PRST_TAG_osc1vibFreq;
    static QString PRST_TAG_osc1vibShape;
    static QString PRST_TAG_envAmp12frame;
    static QString PRST_TAG_envAmp12value;
    static QString PRST_TAG_envFreq12frame;
    static QString PRST_TAG_envFreq12value;
    static QString PRST_TAG_envAmp1sustainPoint;
    static QString PRST_TAG_envFreq1sustainPoint;
    static QString PRST_TAG_envAmp1loop;
    static QString PRST_TAG_envFreq1loop;
    static QString PRST_TAG_envFreq1used;
    static QString PRST_TAG_envFreq1abs;
    static QString PRST_TAG_filterUsed;
    static QString PRST_TAG_filterType;
    static QString PRST_TAG_filterFRatio;
    static QString PRST_TAG_filterQ;
    static QString PRST_TAG_filterFreqEnv1frame;
    static QString PRST_TAG_filterFreqEnv1value;
    static QString PRST_TAG_filterFreqEnvSustainPoint;
    static QString PRST_TAG_filterFreqEnvLoop;
    static QString PRST_TAG_filterFreqEnvUsed;
    static QString PRST_TAG_filterFreqEnvAbs;
};

#endif // MVPRESET_H
