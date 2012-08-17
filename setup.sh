#!/bin/sh
if [ -f Makefile ]
then
	make distclean
fi

qmake -project -o Kaiwa.pro

cat <<- EOL >> Kaiwa.pro

	# Custom Configuration from setup.sh
	unix:DESTDIR = .
	unix:OBJECTS_DIR = .obj
	unix:MOC_DIR = .moc
	unix:RCC_DIR = .rcc
	unix:UI_DIR = .ui
	
	CONFIG += qt
	QT += network

EOL

qmake Kaiwa.pro
