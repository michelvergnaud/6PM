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

#ifndef MVPRESETMANAGER_H
#define MVPRESETMANAGER_H

#include "MVPreset.h"

#include <QDir>
#include <QList>
#include <QMap>

class MVPresetManager
{
public:
    MVPresetManager();
    ~MVPresetManager();
    QStringList banksList() const;
    QStringList presetsList() const;
    QStringList modifiedPresetsList() const;

    void readPresets();
    void addPreset(QString);
    bool setCurrentBank(QString);
    bool setCurrentPreset(QString);
    bool setCurrentBank(int);
    bool setCurrentPreset(int);
    void reloadCurrentPreset();
    bool saveCurrentPreset();
    bool saveCurrentPresetAs(QString);
    bool renameCurrentPreset(QString);
    void deleteCurrentPreset();
    MVPreset * getPresetByName(QString,QString);
    QString getCurrentBank() const {return currentBank;}
    bool renameCurrentBank(QString);
    bool createNewBank(QString);
    void setModified(bool);

    MVPreset * getCurrentPreset() {return currentPreset;}

    const char *getPredefAlgoDescriptor();
    int getPredefAlgoIndex();
    void setPredefAlgoIndex(int);
    QString getUserAlgoDescriptor();
    int getUserAlgoIndex();
    void setUserAlgoIndex(int);

    static const char* predefAlgoDescriptors[NB_PREDEF_ALGOS];
    static QStringList userAlgoDescriptors;
    static int addUserAlgo(QString);
    static int getUserAlgoIndex(QString);

private:
    QString currentBank;
    MVPreset * currentPreset;
    MVPreset * defaultPreset;
    MVPreset * originCurrentPreset;

    QMultiMap<QString, MVPreset*> bankMap;
};

#endif // MVPRESETMANAGER_H
