#! /bin/sh
if [ ! -d ./src ]
then
echo "Dossier src introuvable"
cd ..
exit
fi
echo "cd src"
cd src

echo "Suppression des fichiers de compilation précédente"
rm ../build/*
echo "qmake"

qmake -o ../build/Makefile
if [ $? -ne 0 ]
then
echo "Erreur qmake"
cd ..
exit
fi

echo -n "Compilation..."

make -s -C ../build &
PID=$!
trap ctrl_c INT
function ctrl_c()
{
	kill $PID
  exit      
}
while [ -d /proc/${PID} ]
do
	echo -n "."
	sleep 2
done

echo "."

if [ ! -f ../build/6pm ]
then
echo "Erreur de compilation"
cd ..
exit
fi

echo "cd .."
cd ..

echo "Copie de 6pm dans /usr/local/bin"
sudo cp ./build/6pm /usr/local/bin

if [ -d ~/.6pm ]
then
echo "~/.6pm existe, pas de changement"
else
mkdir ~/.6pm
mkdir ~/.6pm/Presets
mkdir ~/.6pm/MidiMaps
cp -R ./Presets/* ~/.6pm/Presets
cp -R ./MidiMaps/* ~/.6pm/MidiMaps
fi

echo Copie des presets, midimaps et docs dans /usr/local/share

if [ ! -d /usr/local/share/6pm ]
then
sudo mkdir /usr/local/share/6pm
fi 
if [ ! -d /usr/local/share/6pm/Presets ]
then
sudo mkdir /usr/local/share/6pm/Presets
fi
sudo cp -R ./Presets/* /usr/local/share/6pm/Presets

if [ ! -d /usr/local/share/6pm/MidiMaps ]
then
sudo mkdir /usr/local/share/6pm/MidiMaps
fi
sudo cp -R ./MidiMaps/* /usr/local/share/6pm/MidiMaps

if [ ! -d /usr/local/share/6pm/Doc ]
then
sudo mkdir /usr/local/share/6pm/Doc
fi
sudo cp -R ./Doc/6PM_DocFR.pdf /usr/local/share/6pm/Doc

echo "Copie du fichier 6PM_fr.qm dans /usr/local/share:Tanslations"

if [ ! -d /usr/local/share/6pm/Translations ]
then
sudo mkdir /usr/local/share/6pm/Translations
fi
sudo cp ./Translations/6PM_fr.qm /usr/local/share/6pm/Translations

echo "Installation terminée"
