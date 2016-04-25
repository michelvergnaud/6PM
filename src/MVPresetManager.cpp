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

#include "MVPresetManager.h"

#include "MVGlobals.h"
#include "Synth/MVNote.h"

#include <QList>
#include <QSettings>
#include <iostream>

MVPresetManager::MVPresetManager()
{
    Globals::presetManager = this;
    readPresets();
    originCurrentPreset = currentPreset = defaultPreset = new MVPreset();

    QSettings settings;
    int lastBankNum = settings.value("lastBank",0).toInt();
    setCurrentBank(lastBankNum);

    QString lastPreset = settings.value("lastPreset").toString();
    if(lastPreset != "")
        setCurrentPreset(QFileInfo(lastPreset).baseName());
}

MVPresetManager::~MVPresetManager()
{
    delete defaultPreset;
    QList<MVPreset*> list = bankMap.values();
    foreach(MVPreset* p, list)
        delete p;
}

void MVPresetManager::setModified(bool b)
{
    currentPreset->setModified(b);
    originCurrentPreset->setModified(b);
}

bool MVPresetManager::setCurrentBank(int n)
{
    if(n>=0 && n<bankMap.uniqueKeys().size())
        return setCurrentBank(bankMap.uniqueKeys().at(n));
    return false;
}

bool MVPresetManager::setCurrentBank(QString s)
{
    if(QDir(Globals::userPresetsDir).entryList( QDir::Dirs | QDir::NoDotAndDotDot).contains(s))
    {
        currentBank = s;
        QSettings settings;
        settings.setValue("lastBank", bankMap.uniqueKeys().indexOf(s));
        std::cout << "Bank " << currentBank.toStdString() << " selected" << std::endl;
        return true;
    }
    std::cout << "Bank " << s.toStdString() << " not found" << std::endl;
    return false;
}

bool MVPresetManager::renameCurrentBank(QString newName)
{
    QString oldName = currentBank;
    QString oldDirName = Globals::userPresetsDir + "/" + currentBank;
    QString newDirName = Globals::userPresetsDir + "/" + newName;

    if(QFile::rename(oldDirName, newDirName))
    {
        QList<MVPreset*> list = bankMap.values(currentBank);

        foreach(MVPreset * p, list)
            bankMap.insert(newName, p);
        bankMap.remove(oldName);
        currentBank = newName;
        return true;
    }
    return false;
}
bool MVPresetManager::createNewBank(QString name)
{
    if(QDir(Globals::userPresetsDir).mkdir(name))
    {
        setCurrentBank(name);

        return true;
    }
    return false;
}

bool MVPresetManager::setCurrentPreset(int n)
{
    QStringList list = presetsList();
    int nbPresets = list.size();
    if(n >= 0 && n < nbPresets)
        return setCurrentPreset(list.at(n));
    if(n == -1)
        std::cout << "No preset selected" << std::endl;
    else
        std::cout << "Preset number out of range" << std::endl;
    return false;
}

bool MVPresetManager::setCurrentPreset(QString s)
{
    MVPreset * nextPreset = getPresetByName(currentBank, s);
    if(nextPreset)
    {
        if(currentPreset->isModified())
            *originCurrentPreset = *currentPreset;
        *currentPreset = *nextPreset;
        originCurrentPreset = nextPreset;

        MVNote::createAlgo();
        QSettings settings;
        settings.setValue("lastPreset",currentPreset->getFileName());

        std::cout << "Preset " << currentPreset->getName().toStdString() << " selected" << std::endl;
        return true;
    }
    return false;
}

void MVPresetManager::reloadCurrentPreset()
{
    currentPreset->readData();
    *originCurrentPreset = *currentPreset;
    if( ! userAlgoDescriptors.contains(currentPreset->getUserAlgoDescriptor()))
        addUserAlgo(currentPreset->getUserAlgoDescriptor());

    setCurrentPreset(currentPreset->getName());
}

bool MVPresetManager::saveCurrentPreset()
{
    if(currentPreset->saveData())
    {
        *originCurrentPreset = *currentPreset;
        return true;
    }
    return false;
}

bool MVPresetManager::saveCurrentPresetAs(QString newName)
{
    QString newFileName = Globals::userPresetsDir + "/" + currentBank + "/" + newName + PRESET_FILE_EXT;
    MVPreset * existingPreset = getPresetByName(currentBank, newName);
    if(existingPreset)
    {
       existingPreset->copyDataFrom(*currentPreset);
       existingPreset->saveData();
       return true;
    }

    if (currentPreset->saveData(newFileName))
    {
        addPreset(newFileName);
        return true;
    }
    return false;
}
bool MVPresetManager::renameCurrentPreset(QString newName)
{
    if(currentPreset->rename(newName))
    {
        *originCurrentPreset = *currentPreset;
        return true;
    }
    return false;
}

void MVPresetManager::deleteCurrentPreset()
{
    QString bakFile;
    int i=0;
    do
        bakFile = (currentPreset->getFileName() + "%1.bak").arg(i++);
    while(QFile::exists(bakFile));
    QFile::rename(currentPreset->getFileName(), bakFile);
    bankMap.remove(currentBank, originCurrentPreset);
}

void MVPresetManager::addPreset(QString fileName)
{
    MVPreset * newPreset = new MVPreset(fileName);
    bankMap.insert(currentBank, newPreset);
    setCurrentPreset(newPreset->getName());
}

QStringList MVPresetManager::banksList() const
{
    return QDir(Globals::userPresetsDir).entryList( QDir::Dirs | QDir::NoDotAndDotDot);
}

QStringList MVPresetManager::presetsList() const
{
    QStringList list;
    QList<MVPreset*> presets= bankMap.values(currentBank);
    foreach(MVPreset* p, presets)
        list.append(p->getName());
    list.sort();
    return list;
}

QStringList MVPresetManager::modifiedPresetsList() const
{
    QStringList list;
    QStringList banks = banksList();
    foreach (QString bank, banks)
    {
        QList<MVPreset*> presets= bankMap.values(bank);
        foreach(MVPreset* p, presets)
            if(p->isModified())
                list.append(bank + "/" + p->getName());
        list.sort();
    }
    return list;
}

MVPreset * MVPresetManager::getPresetByName(QString bank, QString name)
{
    MVPreset * preset = NULL;
    QList<MVPreset*> list = bankMap.values(bank);
    foreach(MVPreset * p, list)
    {
        if(p->getName() == name)
        {
           preset = p;
           break;
        }
    }
    return preset;
}

void MVPresetManager::readPresets()
{
    QSettings settings;
    userAlgoDescriptors = settings.value("userAlgos").toStringList();

    QList<MVPreset*> list = bankMap.values();
    foreach(MVPreset* p, list)
        delete p;
    bankMap.clear();

    QDir rootDir(Globals::userPresetsDir);
    QFileInfoList bankList = rootDir.entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QFileInfo f, bankList)
    {
        QDir bankDir(f.absoluteFilePath());
        QStringList nameFilter;
        nameFilter << QString("*")+PRESET_FILE_EXT;
        QFileInfoList presetList = bankDir.entryInfoList(nameFilter, QDir::Files);

        foreach(QFileInfo presetF, presetList)
        {
            MVPreset * preset = new MVPreset(presetF.absoluteFilePath());
            bankMap.insert(f.baseName(), preset);
        }
    }
    settings.setValue("userAlgos", userAlgoDescriptors);
}
int MVPresetManager::getUserAlgoIndex()
{
    return getUserAlgoIndex(currentPreset->getUserAlgoDescriptor());
}

const char * MVPresetManager::getPredefAlgoDescriptor()
{
    int i = currentPreset->getPredefAlgoIndex();
    if(i>=0 && i < NB_PREDEF_ALGOS)
        return predefAlgoDescriptors[i];
    return "1";
}

int MVPresetManager::getPredefAlgoIndex()
{
    return currentPreset->getPredefAlgoIndex();
}
void MVPresetManager::setPredefAlgoIndex(int i)
{
    if(i>=0 && i<NB_PREDEF_ALGOS)
    {
        currentPreset->setPredefAlgoIndex(i);
        originCurrentPreset->setPredefAlgoIndex(i);
    }
}

void MVPresetManager::setUserAlgoIndex(int i)
{
    if(i>=0 && i<userAlgoDescriptors.size())
    {
        currentPreset->setUserAlgoDescriptor(userAlgoDescriptors.at(i));
        originCurrentPreset->setUserAlgoDescriptor(userAlgoDescriptors.at(i));
    }
}

QString MVPresetManager::getUserAlgoDescriptor()
{
    return currentPreset->getUserAlgoDescriptor();
}

int MVPresetManager::addUserAlgo(QString descriptor)
{
    if( ! userAlgoDescriptors.contains(descriptor))
            userAlgoDescriptors.append(descriptor.trimmed());
    return userAlgoDescriptors.indexOf(descriptor);
}

int MVPresetManager::getUserAlgoIndex(QString descriptor)
{
    return userAlgoDescriptors.indexOf(descriptor);
}

const char* MVPresetManager::predefAlgoDescriptors[NB_PREDEF_ALGOS] =
    {"13 1*2 3*4 4*5 5*6 6*6", "13 1*2 2*2 3*4 4*5 5*6", "14 1*2 2*3 4*5 5*6 6*6", "14 1*2 2*3 4*5 5*6 6*4", "135 1*2 3*4 5*6 6*6", "135 1*2 3*4 5*6 6*5", "13 1*2 3*4 3*5 5*6 6*6", "13 1*2 3*4 3*5 4*4 5*6",
     "13 1*2 2*2 3*4 3*5 5*6", "14 1*2 2*3 3*3 4*5 4*6", "14 1*2 2*3 4*5 4*6 6*6", "13 1*2 2*2 3*4 3*5 3*6", "13 1*2 3*4 3*5 3*6 6*6", "13 1*2 3*4 4*5 4*6 6*6", "13 1*2 2*2 3*4 4*5 4*6", "1 1*2 1*3 1*5 3*4 5*6 6*6",
     "1 1*2 2*2 1*3 1*5 3*4 5*6", "1 1*2 1*3 3*3 1*4 4*5 5*6", "145 1*2 2*3 4*6 5*6 6*6", "124 1*3 2*3 3*3 4*5 4*6", "1245 1*3 2*3 3*3 4*6 5*6", "1345 1*2 3*6 4*6 5*6 6*6", "1245 2*3 4*6 5*6 6*6", "12345 3*6 4*6 5*6 6*6",
     "12345 4*6 5*6 6*6", "124 2*3 4*5 4*6 6*6", "124 2*3 3*3 4*5 4*6", "136 1*2 3*4 4*5 5*5", "1235 3*4 5*6 6*6", "1236 3*4 4*5 5*5", "12345 5*6 6*6", "123456 6*6"
    };

QStringList MVPresetManager::userAlgoDescriptors;
