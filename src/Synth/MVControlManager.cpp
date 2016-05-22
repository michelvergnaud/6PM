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

#include "MVControlManager.h"

#include "MVControlNames.h"
#include "Synth/MVPlayer.h"
#include "Synth/MVController.h"
#include "UI/MVControllerWidget.h"
#include "MVPresetManager.h"

#include <QSettings>
#include <QFile>

MVControlManager::MVControlManager()
{
    if(! Globals::bHasGui)
    {
        createControllers();
        QSettings settings;
        QString lastMidiMap = settings.value("lastMidiMap").toString();
        loadMidiMap(lastMidiMap);
    }
}
MVControlManager::~MVControlManager()
{}

void MVControlManager::createControllers()
{
    addController(new MVControllerFloat(CTRN_mainVolume, MVPlayer::mainVolume,0.0f, 0.1f));
    for(int i=0;i<NB_OSC;i++)
    {
        addController(new MVControllerFloat(QString(CTRN_osc1midiCC_pan).arg(i), Globals::presetManager->getCurrentPreset()->getOscData(i)->pan, 0.0f, 1.0f));
        addController(new MVControllerFloat(QString(CTRN_osc1midiCC_amp).arg(i), Globals::presetManager->getCurrentPreset()->getOscData(i)->amp, 0.0f, 1.0f));
        addController(new MVControllerFreqRatio(QString(CTRN_osc1midiCC_freqratio).arg(i), Globals::presetManager->getCurrentPreset()->getOscData(i)->freqratio, -48.0f, 48.0f));
        addController(new MVControllerDetune(QString(CTRN_osc1midiCC_detune).arg(i), Globals::presetManager->getCurrentPreset()->getOscData(i)->detune, -1.0f, 1.0f));
        addController(new MVControllerFloat(QString(CTRN_osc1midiCC_random).arg(i), Globals::presetManager->getCurrentPreset()->getOscData(i)->randomize, 0.0f, 0.2f));
        addController(new MVControllerFloat(QString(CTRN_osc1midiCC_iMod).arg(i), Globals::presetManager->getCurrentPreset()->getOscData(i)->iMod,0.0f, 20.0f));
        addController(new MVControllerFloat(QString(CTRN_osc1midiCC_tremAmp).arg(i), Globals::presetManager->getCurrentPreset()->getTremData(i)->amp, 0.0f, 1.0f));
        addController(new MVControllerFloat(QString(CTRN_osc1midiCC_tremFreq).arg(i), Globals::presetManager->getCurrentPreset()->getTremData(i)->freq, 1.0f, 10.0f));
        addController(new MVControllerInt(QString(CTRN_osc1midiCC_tremShape).arg(i), Globals::presetManager->getCurrentPreset()->getTremData(i)->shape, 0, 3));
        addController(new MVControllerFloat(QString(CTRN_osc1midiCC_vibAmp).arg(i), Globals::presetManager->getCurrentPreset()->getVibData(i)->amp, 0.0f, 1.0f));
        addController(new MVControllerFloat(QString(CTRN_osc1midiCC_vibFreq).arg(i), Globals::presetManager->getCurrentPreset()->getVibData(i)->freq, 1.0f, 10.0f));
        addController(new MVControllerInt(QString(CTRN_osc1midiCC_vibShape).arg(i), Globals::presetManager->getCurrentPreset()->getVibData(i)->shape, 0, 3));

        for (int j=0;j<NB_ENV_PTS;j++)
        {
            if(j>0)
            {
                addController(new MVControllerEnvFrame(QString(CTRN_envAmp12midiCC_frame).arg(i).arg(j),
                                                       Globals::presetManager->getCurrentPreset()->getOscAmpEnvData(i)->points[j].frame));
                addController(new MVControllerEnvFrame(QString(CTRN_envFreq12midiCC_frame).arg(i).arg(j),
                                                       Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(i)->points[j].frame));
            }
            if(j<NB_ENV_PTS-1)
            {
                addController(new MVControllerFloat(QString(CTRN_envAmp12midiCC_value).arg(i).arg(j),
                                                    Globals::presetManager->getCurrentPreset()->getOscAmpEnvData(i)->points[j].value, 0.0f, 1.0f));
                addController(new MVControllerFreqValue(QString(CTRN_envFreq12midiCC_value).arg(i).arg(j),
                                                        Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(i)->points[j].value, -48.0f, 48.0f));
            }
        }
        addController(new MVControllerBool(QString(CTRN_envAmp1loopmidiCC).arg(i), Globals::presetManager->getCurrentPreset()->getOscAmpEnvData(i)->loop));
        addController(new MVControllerBool(QString(CTRN_envFreq1loopmidiCC).arg(i), Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(i)->loop));
        addController(new MVControllerBool(QString(CTRN_envFreq1usedmidiCC).arg(i), Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(i)->used));
    }
    addController(new MVControllerBool(QString(CTRN_filterUsedmidiCC), Globals::presetManager->getCurrentPreset()->getFilterData()->used));
    addController(new MVControllerInt(QString(CTRN_filterTypemidiCC), Globals::presetManager->getCurrentPreset()->getFilterData()->type, 0, 3));
    addController(new MVControllerFreqRatio(QString(CTRN_filterFRatiomidiCC), Globals::presetManager->getCurrentPreset()->getFilterData()->fRatio, -48.0f, 48.0f));
    addController(new MVControllerFloat(QString(CTRN_filterQmidiCC), Globals::presetManager->getCurrentPreset()->getFilterData()->q, 0.1f, 10.0f));
    for (int i=0;i<NB_ENV_PTS;i++)
    {
        if(i>0)
            addController(new MVControllerEnvFrame(QString(CTRN_filterFreqEnv1framemidiCC).arg(i),
                                               Globals::presetManager->getCurrentPreset()->getFilterEnvData()->points[i].frame));
        if(i<NB_ENV_PTS-1)
            addController(new MVControllerFloat(QString(CTRN_filterFreqEnv1valuemidiCC).arg(i),
                                                Globals::presetManager->getCurrentPreset()->getFilterEnvData()->points[i].value, 0.0f, 1.0f));
    }
    addController(new MVControllerBool(QString(CTRN_filterFreqEnvLoopmidiCC), Globals::presetManager->getCurrentPreset()->getFilterEnvData()->loop));
    addController(new MVControllerBool(QString(CTRN_filterFreqEnvUsedmidiCC), Globals::presetManager->getCurrentPreset()->getFilterEnvData()->used));
    addController(new MVControllerBool(QString(CTRN_filterFreqEnvAbsmidiCC), Globals::presetManager->getCurrentPreset()->getFilterEnvData()->absolute));
}

void MVControlManager::addController(MVController * c)
{
    controllers.append(c);
}
MVController * MVControlManager::getController(QString name)
{
    for (int i = 0; i < controllers.size(); ++i)
    {
        if (controllers.at(i)->getName() == name)
            return controllers.at(i);
    }
    return NULL;
}

void MVControlManager::processCC(unsigned char controlNum, unsigned char param)
{
    QList<MVController *> list = getAssignedControllers(controlNum);
    for (int i = 0; i < list.size(); ++i)
    {
        MVController * c = list.at(i);
        c->processCC(c->isMidiCCInverted() ? 127-param : param);
    }
}

void MVControlManager::removeAssign(MVController *cw)
{
    assignMap.remove(assignMap.key(cw),cw);
    cw->setState(MVController::NOASSIGN);
}

void MVControlManager::registerAssign(int ctrlNum, MVController* cw)
{
    removeAssign(cw);
    if(ctrlNum > 0)
    {
        assignMap.insert(ctrlNum, cw);
        cw->setState(MVController::ASSIGNED);
    }
}

QList<MVController*> MVControlManager::getAssignedControllers(int ctrlNum)
{
    return assignMap.values(ctrlNum);
}

int MVControlManager::getAssignNum( MVController * controller) const
{
    return assignMap.key(controller);
}

bool MVControlManager::loadMidiMap(QString name)
{
    if(name == QObject::tr("None"))
    {
        for(int i = 0; i<controllers.size();i++)
            removeAssign(controllers.at(i));
    }
    else
    {
        QString fileName = Globals::userMidiMapsDir + "/" + name + MIDIMAP_FILE_EXT;
        if( ! QFile::exists(fileName))
            return false;

        QSettings settings(fileName, QSettings::NativeFormat);

        for(int i = 0; i<controllers.size();i++)
        {
            MVController * c = controllers.at(i);
            if( ! c->getName().isEmpty())
                registerAssign(settings.value(c->getName()).toInt(), c);
        }
    }

    currentMidiMap = name;
    std::cout << "Midimap " << name.toStdString() << " loaded" << std::endl;

    return true;
}
void MVControlManager::saveMidiMap(QString fileName)
{
    if (! fileName.endsWith(MIDIMAP_FILE_EXT))
        fileName += MIDIMAP_FILE_EXT;
    QSettings settings(fileName,QSettings::NativeFormat);

    for(int i = 0; i<controllers.size();i++)
    {
        MVController * c = controllers.at(i);
        settings.setValue(c->getName(), getAssignNum(c));
    }
}
