TEMPLATE = app
CONFIG += qt warn_on debug desktop
DESTDIR = ../bin
TARGET = iqnotes
SOURCES = *.cpp
HEADERS = *.h
DISTFILES = AUTHORS \
             COPYING \
             COPYRIGHT \
             INSTALL \
             THANKS \
             TODO \
             ChangeLog \
             make_arm 
DEFINES = IQNOTES_PICDIR="\""$(IQNOTES_PREFIX)"/share/iqnotes\""
INTERFACES = desktop/ui/*.ui
HEADERS += desktop/qpe/*.h
SOURCES += desktop/qpe/*.cpp
DEFINES += DESKTOP
INCLUDEPATH = desktop desktop/ui
DEPENDPATH = desktop desktop/ui

inst_bin.path = $(IQNOTES_PREFIX)/bin
inst_bin.files = ../bin/iqnotes
inst_doc.path = $(IQNOTES_PREFIX)/share/docs/iqnotes-2.1.0rc1
inst_doc.files = AUTHORS BUGS TODO COPYING COPYRIGHT INSTALL README.DESKTOP THANKS ../help/html/iqnotes.html
inst_pics.path = $(IQNOTES_PREFIX)/share/iqnotes
inst_pics.files = ../pics/*
INSTALLS = inst_bin inst_doc inst_pics
