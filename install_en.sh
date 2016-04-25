#! /bin/sh
if [ ! -d ./src ]
then
echo src folder not found 
cd ..
exit
fi
echo "cd src"
cd src

echo "Deleting previous compilation files"
rm ../build/*
echo "qmake"

qmake -o ../build/Makefile
if [ $? -ne 0 ]
then
echo "qmake error"
cd ..
exit
fi

echo -n Compiling...

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
echo "Compilation error"
cd ..
exit
fi

echo "cd .."
cd ..

echo "Copy 6pm into /usr/local/bin"
sudo cp ./build/6pm /usr/local/bin

if [ -d ~/.6pm ]
then
echo "~/.6pm exists, no change"
else
mkdir ~/.6pm
mkdir ~/.6pm/Presets
mkdir ~/.6pm/MidiMaps
cp -R ./Presets/* ~/.6pm/Presets
cp -R ./MidiMaps/* ~/.6pm/MidiMaps
fi

echo "Copy presets midimaps and user manual into /usr/local/share"

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
sudo cp -R ./Doc/6PM_DocEN.pdf /usr/local/share/6pm/Doc

echo "Installation completed"
