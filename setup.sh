#!/bin/sh
if [ -f Makefile ]
then
	make distclean
fi

qmake -project -o Kaiwa.pro
echo "" >> Kaiwa.pro
echo "# Custom Configuration from setup.sh" >> Kaiwa.pro
echo "unix:DESTDIR = ." >> Kaiwa.pro
echo "unix:OBJECTS_DIR = .obj " >> Kaiwa.pro
echo "unix:MOC_DIR = .moc " >> Kaiwa.pro
echo "unix:RCC_DIR = .rcc " >> Kaiwa.pro
echo "unix:UI_DIR = .ui " >> Kaiwa.pro

qmake Kaiwa.pro
