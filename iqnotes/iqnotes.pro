TEMPLATE = app
desktop:CONFIG += qt release
desktop_debug:CONFIG += qt warn_on debug
qtopia:CONFIG += qtopiaapp 
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

desktop {
    INCLUDEPATH = desktop desktop/ui
    DEPENDPATH = desktop desktop/ui
    INTERFACES = desktop/ui/*.ui
    HEADERS += desktop/qpe/*.h
    SOURCES += desktop/qpe/*.cpp
    DEFINES += DESKTOP IQNOTES_PICDIR="\""$(IQNOTES_PREFIX)"/share/iqnotes\""
    inst_bin.path = $(IQNOTES_PREFIX)/bin
    inst_bin.files = ../bin/iqnotes
    inst_doc.path = $(IQNOTES_PREFIX)/share/docs/iqnotes
    inst_doc.files = AUTHORS BUGS TODO COPYING COPYRIGHT INSTALL README.DESKTOP THANKS ../help/html/iqnotes.html
    inst_pics.path = $(IQNOTES_PREFIX)/share/iqnotes
    inst_pics.files = ../pics/*
    INSTALLS = inst_bin inst_doc inst_pics
}

qtopiaapp {
    CONFIG -= buildQuicklaunch
    QTOPIA_COMPAT_VER=1.5
    INTERFACES=pda/ui/*.ui
}
