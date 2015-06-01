#! /bin/sh
echo Compilation
qmake
make
echo Copie des fichiers
sudo cp ./6pm /usr/local/bin
mkdir ~/.6pm
mkdir ~/.6pm/Presets/
mkdir ~/.6pm/MidiMaps/
cp -R ./Presets/* ~/.6pm/Presets/
cp -R ./MidiMaps/* ~/.6pm/MidiMaps/
