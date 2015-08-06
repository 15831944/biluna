#*****************************************************************
# $Id: defs.pro 2251 2015-06-22 11:56:22Z rutger $
# Created: 2007-01-10
#
# Copyright (C) 2008 Red-Bag. All rights reserved.
# This file is part of the Biluna 'mkspecs' project.
#
# See http://www.red-bag.com for further details.
#*****************************************************************

# qmake project file to include in project files of biluna
# provides settings for release, debug, demo etc.

# UI_DIR overrides both UI_HEADERS_DIR and UI_SOURCES_DIR
UI_DIR	    = ../src/gui/ui
INCLUDEPATH += ../mkspecs

# webkitwidgets includes widgets
QT += core gui help network printsupport sql svg uitools webkitwidgets widgets xml
#CONFIG += release
CONFIG -= release

if (release) {
#    message("Release compile mode in defs.pro")
    QT -= testlib #remove otherwise MS Windows starts with command line
    CONFIG -= debug
    CONFIG -= debug_and_release
    # CONFIG += release
    CONFIG += qt warn_on rtti
    # CONFIG -= thread
    OBJECTS_DIR = ../build_release/obj
    MOC_DIR     = ../build_release/moc
    RCC_DIR     = ../build_release/res

    if (demo) {
        DEFINES += BILUNA_DEMO
        DEFINES += QT_NO_PRINTER
    }
}
else {
#    message("Debug compile mode in defs.pro")
    # QT += testlib
    CONFIG -= release
    CONFIG -= debug_and_release
    CONFIG += debug
    CONFIG += qt warn_on rtti
    # CONFIG -= thread
    OBJECTS_DIR = ../build_debug/obj
    MOC_DIR     = ../build_debug/moc
    RCC_DIR     = ../build_debug/res

    DEFINES += BILUNA_DEBUG

    QT_DEBUG_PLUGINS=1
}

# Target path will be overriden in bil.pro and test applications
CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    message(Debug $$TARGET will be created)
    CONFIG(plugin) {
        # plugin
        DESTDIR = ../lib
        target.path = ../../bil/debug/plugins
        INSTALLS += target
    }
    else {
        CONFIG(RB_DYNLIB) {
            # static or dynamic lib
            DESTDIR = ../lib
            target.path = ../../bil/debug
            INSTALLS += target
        }
        else {
            # executable
            DESTDIR = ../../bil/debug
        }
    }
}
else {
    message(Release $$TARGET will be created)
    CONFIG(plugin) {
                # plugin
        DESTDIR = ../lib
        target.path = ../../bil/release/plugins
        INSTALLS += target
    }
    else {
        CONFIG(RB_DYNLIB) {
            # static or dynamic lib
            DESTDIR = ../lib
            target.path = ../../bil/release
            INSTALLS += target
        }
        else {
            # executable
            DESTDIR = ../../bil/release
        }
    }
}

# Check for host architecture
#message($$QT_CONFIG)
#message($$QMAKE_TARGET.arch)

# OpenSSL
win32 {
!contains(QMAKE_TARGET.arch, x86_64) {
    message("Using OpenSSL-Win32")
    INCLUDEPATH += \
        C:/OpenSSL-Win32/include \
        C:/OpenSSL-Win32/include/openssl

    # ssleay32.dll is old (is now libssl32.dll) but required for webkit
    LIBS += \
        -LC:/OpenSSL-Win32/lib -llibeay32 \
#        -LC:/OpenSSL-Win32/ -llibssl32 \
        -LC:/OpenSSL-Win32/lib -lssleay32

    #openSSL encryption and decryption
    HEADERS += \
        C:/OpenSSL-Win32/include/openssl/evp.h
}
else {
    message("Using OpenSSL-Win64")
    INCLUDEPATH += \
        C:/OpenSSL-Win64/include \
        C:/OpenSSL-Win64/include/openssl

    # ssleay32.dll is old (is now libssl32.dll)?
    LIBS += \
        -LC:/OpenSSL-Win64/lib -llibeay32 \
#        -LC:/OpenSSL-Win32/ -llibssl32 \
        -LC:/OpenSSL-Win64/lib -lssleay32

    #openSSL encryption and decryption
    HEADERS += \
        C:/OpenSSL-Win64/include/openssl/evp.h
}
}

macx {
    CONFIG -= app_bundle
    LIBS += -lssl -lcrypto
}

unix:!macx {
    INCLUDEPATH += /usr/include/openssl
    LIBS += /usr/bin/ -lopenssl
}
