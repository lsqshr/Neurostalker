
TEMPLATE = lib
CONFIG	+= qt plugin warn_off
#CONFIG	+= x86_64
VAA3DPATH = ../../../../v3d_external
INCLUDEPATH	+= $$VAA3DPATH/v3d_main/basic_c_fun
INCLUDEPATH	+= $$VAA3DPATH/v3d_main/comon_lib/include

HEADERS	+= NeuroStalker_plugin.h
HEADERS += utils/vn_imgpreprocess.h

SOURCES	+= NeuroStalker_plugin.cpp
SOURCES += utils/vn_imgpreprocess.cpp

SOURCES	+= $$VAA3DPATH/v3d_main/basic_c_fun/v3d_message.cpp
SOURCES	+= $$VAA3DPATH/v3d_main/basic_c_fun/basic_surf_objs.cpp

TARGET	= $$qtLibraryTarget(NeuroStalker)
DESTDIR	= $$VAA3DPATH/bin/plugins/bigneuronhackathon/NeuroStalker/
