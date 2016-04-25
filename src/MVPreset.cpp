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

#include "MVPreset.h"

#include "MVUtils.h"
#include "Synth/MVPlayer.h"

#include <QFile>
#include <QFileInfo>
#include <QSettings>

MVPreset::MVPreset()
{
    for(int i=0;i<NB_OSC;i++)
    {
        oscData[i] = defaultOscData[i];
        oscAmpEnvData[i] = defaultOscAmpEnvData[i];
        oscFreqEnvData[i] = defaultOscFreqEnvData[i];
        tremData[i] = defaultTremData[i];
        vibData[i] = defaultVibData[i];
    }
    filterData = defaultFilterData;
    filterEnvData = defaultFilterEnvData;
    fileName = "";
    name = "Default";
    bModified = false;
}

MVPreset::MVPreset(QString f)
{
    fileName = f;
    name = QFileInfo(fileName).baseName();
    readData();
    bModified = false;
}
bool MVPreset::rename(QString newName)
{
    QString newFileName = QFileInfo(fileName).absolutePath() + "/" + newName + PRESET_FILE_EXT;
    if (QFile::rename(fileName, newFileName))
    {
        fileName = newFileName;
        name = newName;
        return true;
    }
    return false;
}
void MVPreset::setUserAlgoDescriptor(QString a)
{
    if(a != userAlgoDescriptor)
        bModified = true;
    userAlgoDescriptor = a;
}
void MVPreset::setPredefAlgoIndex(int i)
{
    if(i != predefAlgoIndex)
        bModified = true;
    predefAlgoIndex = i;
}
bool MVPreset::readData()
{
    if (! QFile::exists(fileName))
    {
        std::cout << "File doesnt exist" << std::endl;
        return false;
    }

    QSettings settings(fileName, QSettings::NativeFormat);
    bPredefAlgo = settings.value(PRST_TAG_PreDefinedAlgo,true).toBool();
    if(bPredefAlgo)
    {
        predefAlgoIndex = settings.value(PRST_TAG_numAlgo,0).toInt();
    }
    else
    {
        userAlgoDescriptor = settings.value(PRST_TAG_algoDescriptor,"1").toString();
        userAlgoDescriptor = userAlgoDescriptor.trimmed();
        while(userAlgoDescriptor.contains("  "))
            userAlgoDescriptor.replace("  "," ");
    }
    for(int i=0;i<NB_OSC;i++)
    {
        oscData[i].pan = settings.value(PRST_TAG_osc1pan.arg(i),0).toFloat();
        oscData[i].amp = settings.value(PRST_TAG_osc1amp.arg(i),0.5).toFloat();
        oscData[i].freqratio = settings.value(PRST_TAG_osc1freqratio.arg(i),1.0).toFloat();
        oscData[i].detune = settings.value(PRST_TAG_osc1detune.arg(i),1).toFloat();
        oscData[i].randomize = settings.value(PRST_TAG_osc1random.arg(i),0).toFloat();
        oscData[i].iMod = settings.value(PRST_TAG_osc1iMod.arg(i),0.5).toFloat();

        tremData[i].amp = settings.value(PRST_TAG_osc1tremAmp.arg(i),0).toFloat();
        tremData[i].freq = settings.value(PRST_TAG_osc1tremFreq.arg(i),5).toFloat();
        tremData[i].shape = settings.value(PRST_TAG_osc1tremShape.arg(i),0).toInt();

        vibData[i].amp = settings.value(PRST_TAG_osc1vibAmp.arg(i),0).toFloat();
        vibData[i].freq = settings.value(PRST_TAG_osc1vibFreq.arg(i),5).toFloat();
        vibData[i].shape = settings.value(PRST_TAG_osc1vibShape.arg(i),0).toInt();

        oscFreqEnvData[i].points[0].frame = 0;
        for (int j=0;j<NB_ENV_PTS;j++)
        {
            oscAmpEnvData[i].points[j].frame = msecToFrames(settings.value(PRST_TAG_envAmp12frame.arg(i).arg(j),1000).toInt());
            if(oscAmpEnvData[i].points[j].frame == 0)
                oscAmpEnvData[i].points[j].frame = MVPlayer::getSampleRate()/1000; // Ancient presets
            if(j>0)
            {
                oscFreqEnvData[i].points[j].frame = msecToFrames(settings.value(PRST_TAG_envFreq12frame.arg(i).arg(j),1000).toInt());
                if(oscFreqEnvData[i].points[j].frame == 0)
                    oscFreqEnvData[i].points[j].frame = MVPlayer::getSampleRate()/1000; // Anciens presets
            }
            if(j<NB_ENV_PTS-1)
            {
                oscAmpEnvData[i].points[j].value = settings.value(PRST_TAG_envAmp12value.arg(i).arg(j),0.5).toFloat();
                oscFreqEnvData[i].points[j].value = settings.value(PRST_TAG_envFreq12value.arg(i).arg(j),1).toFloat();
            }
        }
        oscAmpEnvData[i].sustainPoint = settings.value(PRST_TAG_envAmp1sustainPoint.arg(i),3).toInt();
        oscFreqEnvData[i].sustainPoint = settings.value(PRST_TAG_envFreq1sustainPoint.arg(i),3).toInt();
        oscAmpEnvData[i].loop = settings.value(PRST_TAG_envAmp1loop.arg(i),false).toBool();
        oscFreqEnvData[i].loop = settings.value(PRST_TAG_envFreq1loop.arg(i),false).toBool();
        oscFreqEnvData[i].used = settings.value(PRST_TAG_envFreq1used.arg(i),true).toBool();
        oscFreqEnvData[i].absolute = settings.value(PRST_TAG_envFreq1abs.arg(i),false).toBool();;

        oscAmpEnvData[i].points[NB_ENV_PTS-1].value = 0.0;
        oscFreqEnvData[i].points[NB_ENV_PTS-1].value = 1.0;
    }

    filterData.used = settings.value(PRST_TAG_filterUsed,false).toBool();
    filterData.type = settings.value(PRST_TAG_filterType,0).toInt();
    filterData.fRatio = settings.value(PRST_TAG_filterFRatio,1.0).toFloat();
    filterData.q = settings.value(PRST_TAG_filterQ,0.5).toFloat();

    for (int i=0;i<NB_ENV_PTS;i++)
    {
        filterEnvData.points[i].frame = msecToFrames(settings.value(PRST_TAG_filterFreqEnv1frame.arg(i),1000).toInt());
        filterEnvData.points[i].value = settings.value(PRST_TAG_filterFreqEnv1value.arg(i),1.0).toFloat();
    }
    filterEnvData.points[0].frame = 0.0;

    filterEnvData.sustainPoint = settings.value(PRST_TAG_filterFreqEnvSustainPoint,4).toInt();
    filterEnvData.loop = settings.value(PRST_TAG_filterFreqEnvLoop,false).toBool();
    filterEnvData.used = settings.value(PRST_TAG_filterFreqEnvUsed,true).toBool();
    filterEnvData.absolute = settings.value(PRST_TAG_filterFreqEnvAbs,false).toBool();

    bModified = false;
    return true;
}
bool MVPreset::saveData()
{
    if(saveData(fileName))
    {
        setModified(false);
        return true;
    }
    return false;
}

bool MVPreset::saveData(QString fName)
{
    QSettings settings(fName,QSettings::NativeFormat);
    settings.setValue(PRST_TAG_PreDefinedAlgo, bPredefAlgo);
    if (bPredefAlgo)
        settings.setValue(PRST_TAG_numAlgo, predefAlgoIndex);
    else
        settings.setValue(PRST_TAG_algoDescriptor, userAlgoDescriptor);

    for(int i=0;i<NB_OSC;i++)
    {
        settings.setValue(PRST_TAG_osc1pan.arg(i), oscData[i].pan);
        settings.setValue(PRST_TAG_osc1amp.arg(i), oscData[i].amp);
        settings.setValue(PRST_TAG_osc1freqratio.arg(i), oscData[i].freqratio);
        settings.setValue(PRST_TAG_osc1detune.arg(i), oscData[i].detune);
        settings.setValue(PRST_TAG_osc1random.arg(i), oscData[i].randomize);
        settings.setValue(PRST_TAG_osc1iMod.arg(i), oscData[i].iMod);
        settings.setValue(PRST_TAG_osc1tremAmp.arg(i), tremData[i].amp);
        settings.setValue(PRST_TAG_osc1tremFreq.arg(i), tremData[i].freq);
        settings.setValue(PRST_TAG_osc1tremShape.arg(i), tremData[i].shape);
        settings.setValue(PRST_TAG_osc1vibAmp.arg(i), vibData[i].amp);
        settings.setValue(PRST_TAG_osc1vibFreq.arg(i), vibData[i].freq);
        settings.setValue(PRST_TAG_osc1vibShape.arg(i), vibData[i].shape);

        for (int j=0;j<NB_ENV_PTS;j++)
        {
           settings.setValue(PRST_TAG_envAmp12frame.arg(i).arg(j), framesTomsec(oscAmpEnvData[i].points[j].frame));
           settings.setValue(PRST_TAG_envAmp12value.arg(i).arg(j), oscAmpEnvData[i].points[j].value);
           settings.setValue(PRST_TAG_envFreq12frame.arg(i).arg(j), framesTomsec(oscFreqEnvData[i].points[j].frame));
           settings.setValue(PRST_TAG_envFreq12value.arg(i).arg(j), oscFreqEnvData[i].points[j].value);
        }
        settings.setValue(PRST_TAG_envAmp1sustainPoint.arg(i), oscAmpEnvData[i].sustainPoint);
        settings.setValue(PRST_TAG_envFreq1sustainPoint.arg(i), oscFreqEnvData[i].sustainPoint);
        settings.setValue(PRST_TAG_envAmp1loop.arg(i), oscAmpEnvData[i].loop);
        settings.setValue(PRST_TAG_envFreq1loop.arg(i), oscFreqEnvData[i].loop);
        settings.setValue(PRST_TAG_envFreq1used.arg(i), oscFreqEnvData[i].used);
        settings.setValue(PRST_TAG_envFreq1abs.arg(i), oscFreqEnvData[i].absolute);

    }
    settings.setValue(PRST_TAG_filterUsed, filterData.used);
    settings.setValue(PRST_TAG_filterType, filterData.type);
    settings.setValue(PRST_TAG_filterFRatio, filterData.fRatio);
    settings.setValue(PRST_TAG_filterQ, filterData.q);
    for (int i=0;i<NB_ENV_PTS;i++)
    {
        settings.setValue(PRST_TAG_filterFreqEnv1frame.arg(i), framesTomsec(filterEnvData.points[i].frame));
        settings.setValue(PRST_TAG_filterFreqEnv1value.arg(i), filterEnvData.points[i].value);
    }
    settings.setValue(PRST_TAG_filterFreqEnvSustainPoint, filterEnvData.sustainPoint);
    settings.setValue(PRST_TAG_filterFreqEnvLoop, filterEnvData.loop);
    settings.setValue(PRST_TAG_filterFreqEnvUsed, filterEnvData.used);
    settings.setValue(PRST_TAG_filterFreqEnvAbs, filterEnvData.absolute);

    return true;
}

void MVPreset::copyDataFrom(MVPreset &other)
{
    QString saveName = name;
    QString saveFilename = fileName;
    *this = other;
    name = saveName;
    fileName = saveFilename;
}
void MVPreset::setModified(bool b)
{
    bModified = b;
}

OscillatorData MVPreset::defaultOscData[NB_OSC]=
{
     {0.0,0.3,1.0,1.0,5.0,1.0,0.0},
     {0.0,0.3,1.0,1.0,5.0,1.0,0.0},
     {0.0,0.3,1.0,1.0,5.0,1.0,0.0},
     {0.0,0.3,1.0,1.0,5.0,1.0,0.0},
     {0.0,0.3,1.0,1.0,5.0,1.0,0.0},
     {0.0,0.3,1.0,1.0,5.0,1.0,0.0}
};

LfoData MVPreset::defaultTremData[NB_OSC]=
    {{0.0,5.0,0},{0.0,5.0,0},{0.0,5.0,0},{0.0,5.0,0},{0.0,5.0,0},{0.0,5.0,0}};

LfoData MVPreset::defaultVibData[NB_OSC]=
    {{0.0,5.0,0},{0.0,5.0,0},{0.0,5.0,0},{0.0,5.0,0},{0.0,5.0,0},{0.0,5.0,0}};


AmpEnvelopeData MVPreset::defaultOscAmpEnvData[NB_OSC] =
{    (EnvelopePoint){96,0},(EnvelopePoint){4000,1},(EnvelopePoint){3000,1},(EnvelopePoint){2000,1},(EnvelopePoint){1000,0.5},(EnvelopePoint){1000,0.0}, (int)3 ,(bool)false,
     (EnvelopePoint){96,0},(EnvelopePoint){4000,1},(EnvelopePoint){3000,1},(EnvelopePoint){2000,1},(EnvelopePoint){1000,0.5},(EnvelopePoint){1000,0.0}, (int)3 ,(bool)false,
     (EnvelopePoint){96,0},(EnvelopePoint){4000,1},(EnvelopePoint){3000,1},(EnvelopePoint){2000,1},(EnvelopePoint){1000,0.5},(EnvelopePoint){1000,0.0}, (int)3 ,(bool)false,
     (EnvelopePoint){96,0},(EnvelopePoint){4000,1},(EnvelopePoint){3000,1},(EnvelopePoint){2000,1},(EnvelopePoint){1000,0.5},(EnvelopePoint){1000,0.0}, (int)3 ,(bool)false,
     (EnvelopePoint){96,0},(EnvelopePoint){4000,1},(EnvelopePoint){3000,1},(EnvelopePoint){2000,1},(EnvelopePoint){1000,0.5},(EnvelopePoint){1000,0.0}, (int)3 ,(bool)false,
     (EnvelopePoint){96,0},(EnvelopePoint){4000,1},(EnvelopePoint){3000,1},(EnvelopePoint){2000,1},(EnvelopePoint){1000,0.5},(EnvelopePoint){1000,0.0}, (int)3 ,(bool)false
};

FreqEnvelopeData MVPreset::defaultOscFreqEnvData[NB_OSC] =
{
    (EnvelopePoint){0,1.0},(EnvelopePoint){22000,1.0},(EnvelopePoint){44000,1.0},(EnvelopePoint){33000,1.0},(EnvelopePoint){960,1.0},(EnvelopePoint){480,1.0},(int)3,(bool)false,(bool)true,(bool)false,
    (EnvelopePoint){0,1.0},(EnvelopePoint){22000,1.0},(EnvelopePoint){44000,1.0},(EnvelopePoint){33000,1.0},(EnvelopePoint){960,1.0},(EnvelopePoint){480,1.0},(int)3,(bool)false,(bool)true,(bool)false,
    (EnvelopePoint){0,1.0},(EnvelopePoint){22000,1.0},(EnvelopePoint){44000,1.0},(EnvelopePoint){33000,1.0},(EnvelopePoint){960,1.0},(EnvelopePoint){480,1.0},(int)3,(bool)false,(bool)true,(bool)false,
    (EnvelopePoint){0,1.0},(EnvelopePoint){22000,1.0},(EnvelopePoint){44000,1.0},(EnvelopePoint){33000,1.0},(EnvelopePoint){960,1.0},(EnvelopePoint){480,1.0},(int)3,(bool)false,(bool)true,(bool)false,
    (EnvelopePoint){0,1.0},(EnvelopePoint){22000,1.0},(EnvelopePoint){44000,1.0},(EnvelopePoint){33000,1.0},(EnvelopePoint){960,1.0},(EnvelopePoint){480,1.0},(int)3,(bool)false,(bool)true,(bool)false,
    (EnvelopePoint){0,1.0},(EnvelopePoint){22000,1.0},(EnvelopePoint){44000,1.0},(EnvelopePoint){33000,1.0},(EnvelopePoint){960,1.0},(EnvelopePoint){480,1.0},(int)3,(bool)false,(bool)true,(bool)false
 };

FilterData MVPreset::defaultFilterData= {false,0,1,1.0};

FreqEnvelopeData MVPreset::defaultFilterEnvData =
{
   (EnvelopePoint){0,1.0},
   (EnvelopePoint){24000,1.0},
   (EnvelopePoint){48000,1.0},
   (EnvelopePoint){96000,1.0},
   (EnvelopePoint){1000,1.0},
   (EnvelopePoint){1000,1.0},
   (int)3,(bool)false,(bool)true,(bool)false
};

QString MVPreset::PRST_TAG_PreDefinedAlgo = "PreDefinedAlgo";
QString MVPreset::PRST_TAG_numAlgo = "numAlgo";
QString MVPreset::PRST_TAG_algoDescriptor= "algoDescriptor";
QString MVPreset::PRST_TAG_osc1pan = "osc%1pan";
QString MVPreset::PRST_TAG_osc1amp = "osc%1amp";
QString MVPreset::PRST_TAG_osc1freqratio = "osc%1freqratio";
QString MVPreset::PRST_TAG_osc1detune = "osc%1detune";
QString MVPreset::PRST_TAG_osc1random = "osc%1random";
QString MVPreset::PRST_TAG_osc1iMod = "osc%1iMod";
QString MVPreset::PRST_TAG_osc1tremAmp = "osc%1tremAmp";
QString MVPreset::PRST_TAG_osc1tremFreq = "osc%1tremFreq";
QString MVPreset::PRST_TAG_osc1tremShape = "osc%1tremShape";
QString MVPreset::PRST_TAG_osc1vibAmp = "osc%1vibAmp";
QString MVPreset::PRST_TAG_osc1vibFreq = "osc%1vibFreq";
QString MVPreset::PRST_TAG_osc1vibShape = "osc%1vibShape";
QString MVPreset::PRST_TAG_envAmp12frame = "envAmp%1%2frame";
QString MVPreset::PRST_TAG_envAmp12value = "envAmp%1%2value";
QString MVPreset::PRST_TAG_envFreq12frame = "envFreq%1%2frame";
QString MVPreset::PRST_TAG_envFreq12value = "envFreq%1%2value";
QString MVPreset::PRST_TAG_envAmp1sustainPoint = "envAmp%1sustainPoint";
QString MVPreset::PRST_TAG_envFreq1sustainPoint = "envFreq%1sustainPoint";
QString MVPreset::PRST_TAG_envAmp1loop = "envAmp%1loop";
QString MVPreset::PRST_TAG_envFreq1loop = "envFreq%1loop";
QString MVPreset::PRST_TAG_envFreq1used = "envFreq%1used";
QString MVPreset::PRST_TAG_envFreq1abs = "envFreq%1abs";
QString MVPreset::PRST_TAG_filterUsed = "filterUsed";
QString MVPreset::PRST_TAG_filterType = "filterType";
QString MVPreset::PRST_TAG_filterFRatio = "filterFRatio";
QString MVPreset::PRST_TAG_filterQ = "filterQ";
QString MVPreset::PRST_TAG_filterFreqEnv1frame = "filterFreqEnv%1frame";
QString MVPreset::PRST_TAG_filterFreqEnv1value = "filterFreqEnv%1value";
QString MVPreset::PRST_TAG_filterFreqEnvSustainPoint = "filterFreqEnvSustainPoint";
QString MVPreset::PRST_TAG_filterFreqEnvLoop = "filterFreqEnvLoop";
QString MVPreset::PRST_TAG_filterFreqEnvUsed = "filterFreqEnvUsed";
QString MVPreset::PRST_TAG_filterFreqEnvAbs = "filterFreqEnvAbs";
