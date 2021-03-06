# ----------------------------------------------------------
# $Id: cad.pro 0001 2016-08-19  rutger $
# Created: Aug 19, 2016 - rutger
#
# Copyright (C) 2016 Red-Bag. All rights reserved.
# This file is part of the Biluna CAD project.
#
# See http://www.red-bag.com for further details.
#
# Comments:
# - for building cad lib
# ----------------------------------------------------------

# Type of binary, name and destination
TEMPLATE = lib
LANGUAGE = C++
TARGET   = cad

CONFIG += openssl-linked
CONFIG += shared

# Biluna specific
CONFIG += RB_DYNLIB
DEFINES += CAD_DLL

# Compiler settings for release, debug and path for ui, moc, binary files
exists( ../../mkspecs/defs.pro ) {
    include( ../../mkspecs/defs.pro )
}
else {
    # message and exit
    error( "An error has occurred, ../../mkspecs/defs.pro is missing." )
}

INCLUDEPATH += \
    ../../cad/src/core \
    ../../thrd/qcad2/dxflib/src \
    ../../thrd/qcad2/fparser/src \
    ../../thrd/qcad2/qcadactions/src/blocks \
    ../../thrd/qcad2/qcadactions/src/debugging \
    ../../thrd/qcad2/qcadactions/src/dimension \
    ../../thrd/qcad2/qcadactions/src/draw \
    ../../thrd/qcad2/qcadactions/src/edit \
    ../../thrd/qcad2/qcadactions/src/file \
    ../../thrd/qcad2/qcadactions/src/info \
    ../../thrd/qcad2/qcadactions/src/layers \
    ../../thrd/qcad2/qcadactions/src/misc \
    ../../thrd/qcad2/qcadactions/src/modify \
    ../../thrd/qcad2/qcadactions/src/options \
    ../../thrd/qcad2/qcadactions/src/script \
    ../../thrd/qcad2/qcadactions/src/select \
    ../../thrd/qcad2/qcadactions/src/snap \
    ../../thrd/qcad2/qcadactions/src/view \
    ../../thrd/qcad2/qcadactions/src/zoom \
    ../../thrd/qcad2/qcadguiqt/src \
    ../../thrd/qcad2/qcadguiqt/src/dialogs \
    ../../thrd/qcad2/qcadguiqt/src/toolbars \
    ../../thrd/qcad2/qcadguiqt/src/tooloptions \
    ../../thrd/qcad2/qcadguiqt/src/widgets \
    ../../thrd/qcad2/qcadlib/src/actions \
    ../../thrd/qcad2/qcadlib/src/creation \
    ../../thrd/qcad2/qcadlib/src/entities \
    ../../thrd/qcad2/qcadlib/src/fileio\
    ../../thrd/qcad2/qcadlib/src/filters \
    ../../thrd/qcad2/qcadlib/src/global \
    ../../thrd/qcad2/qcadlib/src/gui \
    ../../thrd/qcad2/qcadlib/src/information \
    ../../thrd/qcad2/qcadlib/src/math \
    ../../thrd/qcad2/qcadlib/src/modification \
    ../../thrd/qcad2/qcadlib/src/scripting \
    ../../thrd/qcad2/qcadlib/src/tools \
    ../../thrd/qcad2/qcadprop \
    ../../thrd/qcad2/qcadsvg/src

# LIBS		+=  -lmysqlclient -L/usr/lib/mysql/

# Add forms, headers and sources
exists( cad.pri ) {
    include( cad.pri )
} else {
    # message and exit
    error( "An error has occurred, cad.pri is missing." )
}

# In case of release remove header and source debug files from .pri
#CONFIG (release) {
#    HEADERS -= \
#        ../src/utils/rb_signalspydialog.h
#    SOURCES -= \
#        ../src/utils/rb_signalspydialog.cpp
#}
