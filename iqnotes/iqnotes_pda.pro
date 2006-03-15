TEMPLATE = app
CONFIG = qt warn_on pda
#CONFIG = qt warn_on debug desktop
DEFINES = TOOLBAR_BIG_ICONS
pda:INCLUDEPATH += $(QPEDIR)/include
DESTDIR = ../bin
pda:DEPENDPATH += $(QPEDIR)/include
pda:LIBS += -lqtopia -lqpe
TARGET = iqnotes
SOURCES += main.cpp \
           app.cpp \
           notes.cpp \
           defineEntry.cpp \
           entry.cpp \
           noteType.cpp \
           writeDefinedEntry.cpp \
           notesFile.cpp \
           chooseEntry.cpp \
           colorDialog.cpp \
           qsketch.cpp \
           writeSketchNote.cpp \
           setTask.cpp \
           setEvent.cpp \
           sort.cpp \
           choosePic.cpp \
           rijndael.cpp \
           md5.cpp \
           preferences.cpp \
           openFile.cpp \
           fileName.cpp \
           setReminder.cpp \
           cfgFile.cpp 
HEADERS += app.h \
           notes.h \
           defineEntry.h \
           entry.h \
           noteType.h \
           writeDefinedEntry.h \
           notesFile.h \
           chooseEntry.h \
           colorDialog.h \
           qsketch.h \
           writeSketchNote.h \
           setTask.h \
           setEvent.h \
           sort.h \
           choosePic.h \
           rijndael.h \
           md5.h \
           preferences.h \
           openFile.h \
           fileName.h \
           setReminder.h \
           notesConfig.h \
           cfgFile.h \
           toolBarIcon.h 
DISTFILES += AUTHORS \
             COPYING \
             COPYRIGHT \
             INSTALL \
             THANKS \
             TODO \
             ChangeLog \
             make_arm 
desktop:DEFINES = IQNOTES_PICDIR="\""$(IQNOTES_PREFIX)"/usr/share/iqnotes\""
desktop:INTERFACES = desktop/ui/*.ui
desktop:HEADERS += desktop/qpe/*.h
desktop:SOURCES += desktop/qpe/*.cpp
desktop:DEFINES += DESKTOP
desktop:INCLUDEPATH = desktop desktop/ui
desktop:DEPENDPATH = desktop desktop/ui
#desktop:LIBS = -lfreetype

pda:INTERFACES = aboutBase.ui          choosePicBase.ui     preferencesBase.ui  setTaskBase.ui            writeNoteBase.ui \
  defineEntryBase.ui    iqnotesAlarmBase.ui  renameNoteBase.ui   sortBase.ui \
  deleteConfirmBase.ui  noteNameBase.ui      searchBase.ui       sortTasksBase.ui \
  fileNameBase.ui       noteTypeBase.ui      setEventBase.ui     startUpPasswdBase.ui \
  chooseEntryBase.ui    openFileBase.ui      setReminderBase.ui  writeDefinedEntryBase.ui
pda:LIBS += -luuid -ljpeg -lstdc++
#pda:LIBS += -lstdc++
