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

#include "MVCommandProcessor.h"

#include "MVGlobals.h"
#include "Synth/MVPlayer.h"
#include "MVPresetManager.h"
#include "Synth/MVControlManager.h"

#include <iostream>
#include <fstream>

MVCommandProcessor::MVCommandProcessor()
{
    delim=' ';
}
void MVCommandProcessor::doProcess()
{
    do
    {
        std::cout << "Enter a command (h for help) : ";
        std::cin.getline(cmdLine,255);
        processLine();
    }
    while (strcmp(cmdLine,"q") != 0);
}

void MVCommandProcessor::processLine()
{
    QString str(cmdLine);
    str = str.trimmed();

    bool ok;

    if(str.startsWith('h'))
    {
        printHelp();
    }
    else
    if(str.startsWith('p'))
    {
        str.remove(0,1);
        if( ! str.isEmpty())
        {
            int n = str.toInt(&ok);
            if(ok) Globals::presetManager->setCurrentPreset(n);
            else std::cout << "Incorrect argument" << std::endl;
        }
        else
        {
            std::cout << "Current preset : " << Globals::presetManager->getCurrentPreset()->getName().toStdString() << std::endl;
        }
    }
    else
    if(str.startsWith('b'))
    {
        str.remove(0,1);
        if( ! str.isEmpty())
        {
            int n = str.toInt(&ok);
            if(ok) Globals::presetManager->setCurrentBank(n);
            else std::cout << "Incorrect argument" << std::endl;
        }
        else
        {
            std::cout << "Current bank : " << Globals::presetManager->getCurrentBank().toStdString() << std::endl;
        }
    }
    else
    if(str.startsWith("lsp"))
    {
        printListPresets();
    }
    else
    if(str.startsWith("lsb"))
    {
        printListBanks();
    }
    else
    if(str.startsWith('v'))
    {
        str.remove(0,1);
        if( ! str.isEmpty())
        {
            int n = str.toInt(&ok);
            if(ok && n>=0 && n<=100)
            {
                MVPlayer::mainVolume = (float)n/100.0;
                std::cout << "Volume set to " << n << std::endl;
            }
            else std::cout << "Incorrect argument" << std::endl;
        }
        else
        {
            std::cout << "Volume : " << (int)(MVPlayer::mainVolume * 100) << std::endl;
        }
    }
    else
    if(str.startsWith('f'))
    {
        str.remove(0,1);
        str = str.trimmed();
        if( ! str.isEmpty())
        {
            if(QFile::exists(str))
            {
                std::cout << "Reading " << str.toStdString() << std::endl;
                std::ifstream infile(str.toLatin1());
                std::string line;
                while (std::getline(infile, line))
                {std::cout << line << std::endl;
                    strcpy(cmdLine, line.c_str());
                    processLine();
                }
            }
            else std::cout << "File not found" << std::endl;
        }
        else
        {
            std::cout << "Incorrect file name" << std::endl;
        }
    }
    else
    if(str.startsWith('s'))
    {
        str.remove(0,1);
        str = str.trimmed();
        if( ! str.isEmpty())
        {
            QString fileName = Globals::userPresetsDir + "/" + str + PRESET_FILE_EXT;
            if( ! QFile::exists(fileName))
                std::cout << "Saved as " << fileName.toStdString() << std::endl;
            else std::cout << "File already existing, use another name" << std::endl;
        }
        else
        {
            Globals::presetManager->saveCurrentPreset();
        }
    }
    else
    if(str.startsWith("lsmm"))
    {
        printListMidiMaps();
    }
    if(str.startsWith("mm"))
    {
        str.remove(0,2);
        str = str.trimmed();
        if( ! str.isEmpty())
        {
            if( ! Globals::controlmanager->loadMidiMap(str))
                std::cout << "Impossible to load midimap " << str.toStdString() << std::endl;
        }
        else
        {
            std::cout << "Current midi map : " << Globals::controlmanager->getCurrentMidiMap().toStdString() << std::endl;
        }
    }
}

void MVCommandProcessor::printListPresets()
{
    QStringList list = Globals::presetManager->presetsList();
    for(int i=0;i<list.count();i++)
        std::cout << i << " : " << list.at(i).toStdString() << std::endl;
}
void MVCommandProcessor::printListBanks()
{
    QStringList list = Globals::presetManager->banksList();
    for(int i=0;i<list.count();i++)
        std::cout << i << " : " << list.at(i).toStdString() << std::endl;
}
void MVCommandProcessor::printListMidiMaps()
{
    QStringList nameFilter;
    nameFilter << QString("*")+MIDIMAP_FILE_EXT;

    QFileInfoList mmList = QDir(Globals::userMidiMapsDir).entryInfoList( nameFilter, QDir::Files);

    foreach(QFileInfo f, mmList)
        std::cout  << f.baseName().toStdString() << std::endl;
    std::cout << std::endl;
}
void MVCommandProcessor::printHelp()
{
    std::cout << "------------" << std::endl
              << "6pm commands" << std::endl
              << "------------" << std::endl << std::endl
              << " h   ----------> help" << std::endl
              << " lsp ----------> list presets" << std::endl
              << " lsb ----------> list banks" << std::endl
              << " p   ----------> show current preset" << std::endl
              << " p n ----------> select preset n" << std::endl
              << " b   ----------> show current bank banks" << std::endl
              << " b n ----------> select bank n" << std::endl
              << " v   ----------> print main volume" << std::endl
              << " v n ----------> set mainvolume to n [0..100]" << std::endl
              << " f filename ---> process commands file \"filename\"" << std::endl
              << " s          ---> save current preset" << std::endl
              << " s name     ---> save current preset as \"name\"" << std::endl
              << " lsmm       ---> list midi maps" << std::endl
              << " mm         ---> show current midi map" << std::endl
              << " mm name    ---> load midi map \"name\"" << std::endl
              << " q   ----------> quit " << std::endl << std::endl;
}
