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


#include "UI/MVMainWindow.h"
#include "UI/MVControlUIManager.h"
#include "Synth/MVPlayer.h"
#include "Synth/MVControlManager.h"
#include "MVPresetManager.h"
#include "MVCommandProcessor.h"
#include "MVGlobals.h"
#include "MVVersion.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLocale locale;
    QTranslator qtTranslator;
    if(locale.language() == QLocale::French)
    {
        QString frFile = "6PM_fr";
        QString frDir = "/usr/share/6pm/Translations";
        if ( ! qtTranslator.load(frFile,frDir))
            frDir = "/usr/local/share/6pm/Translations";
        qtTranslator.load(frFile,frDir);
        app.installTranslator(&qtTranslator);
    }

    QCoreApplication::setOrganizationName("MVSoft");
    QCoreApplication::setApplicationName("6PM");

    int retval = 0;

    Globals::bHasGui = true;
    bool askForHelp = false;
    for(int i=0;i<argc;i++)
    {
        if (strcmp(argv[i],"--nogui")==0)
            Globals::bHasGui = false;
        if (strcmp(argv[i],"--help") == 0 || strcmp(argv[i],"-h")==0)
            askForHelp = true;
    }

    if (askForHelp)
    {
        std::cout << "--------------------------------------------------------------------" << std::endl
                  << "6PM version " << MV6PM_VERSION << std::endl
                  << std::endl
                  << "6pm [options]"  << std::endl
                  << std::endl
                  << "Otions : "  << std::endl
                  << std::endl
                  << "-h  --help    : prints this help"   << std::endl
                  << "    --nogui   : runs without graphical interface"  << std::endl
                  << std::endl
                  << QObject::tr("User manual : see Help menu").toStdString() << std::endl
                  << QObject::tr("or :").toStdString() << std::endl
                  << "https://sourceforge.net/projects/mv-6pm/files/6PM_DocEN.pdf/download" << std::endl
                  << "https://sourceforge.net/projects/mv-6pm/files/6PM_DocFR.pdf/download" << std::endl
                  << "--------------------------------------------------------------------" << std::endl
                  << std::endl;

        exit(0);
    }
    std::cout << QObject::tr("Checking directory ~/.6pm").toStdString() << std::endl;

    if (! QDir(Globals::userDir).exists())
    {
        int notok = system("mkdir ~/.6pm");

        if(notok)
            std::cout << QObject::tr("Failed to create directory ~/.6pm").toStdString() << std::endl;
        else
        {
            std::cout << QObject::tr("Directory  ~/.6pm created").toStdString() << std::endl;
            std::cout << QObject::tr("Copying files").toStdString() << std::endl;
            notok = system("cp -R /usr/share/6pm/* ~/.6pm");
        }
        if(notok)
            std::cout << QObject::tr("Failed to copy presets and midimaps files").toStdString() << std::endl;
        else
            std::cout << "Ok" << std::endl;
    }

    Globals::presetManager = new MVPresetManager();
    Globals::player = new MVPlayer();

    MVMainWindow * w = NULL;
    MVCommandProcessor * cp = NULL;
    if(Globals::bHasGui)
    {
        Globals::controlmanager = new MVControlUIManager();
        w = new MVMainWindow();
        w->show();
        retval = app.exec();
    }
    else
    {
        Globals::controlmanager = new MVControlManager();
        cp = new MVCommandProcessor();
        cp->doProcess();
    }

    if(w) delete w;
    if(cp) delete cp;
    return retval;
}
