
TEMPLATE = lib
CONFIG	+= qt plugin warn_off
#CONFIG	+= x86_64
#CONFIG += use_static_libs
VAA3DPATH = /home/siqi/workspace/v3d_external
INCLUDEPATH	+= $$VAA3DPATH/v3d_main/basic_c_fun
INCLUDEPATH	+= $$VAA3DPATH/v3d_main/common_lib/include

#OpenCV headers and library folders
INCLUDEPATH += /usr/local/include/opencv
LIBS+= -L/usr/local/lib

#LIBS+= -lopencv_core -lopencv_imgproc -lopencv_highgui \
#		-lzlib \
#		-llibtiff \
#		-llibjpeg \
#		-lIlmImf \
#		-llibjasper

HEADERS	+= NeuroStalker_plugin.h
HEADERS	+= Stalker.h
SOURCES	+= NeuroStalker_plugin.cpp
SOURCES	+= Stalker.cpp
SOURCES	+= $$VAA3DPATH/v3d_main/basic_c_fun/v3d_message.cpp
SOURCES	+= $$VAA3DPATH/v3d_main/basic_c_fun/basic_surf_objs.cpp


TARGET	= $$qtLibraryTarget(NeuroStalker)
DESTDIR	= $$VAA3DPATH/bin/plugins/bigneuronhackathon/NeuroStalker/
