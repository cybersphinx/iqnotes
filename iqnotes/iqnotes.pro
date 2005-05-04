TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
#DEFINES		= DEMO
HEADERS		= app.h notes.h defineEntry.h entry.h noteType.h writeDefinedEntry.h notesFile.h \
		    chooseEntry.h colorDialog.h qsketch.h writeSketchNote.h setTask.h \
                    setEvent.h sort.h choosePic.h rijndael.h md5.h preferences.h openFile.h \
                    fileName.h setReminder.h notesConfig.h cfgFile.h
SOURCES		= main.cpp app.cpp notes.cpp defineEntry.cpp entry.cpp noteType.cpp writeDefinedEntry.cpp \
		    notesFile.cpp chooseEntry.cpp colorDialog.cpp qsketch.cpp writeSketchNote.cpp \
                    setTask.cpp setEvent.cpp sort.cpp choosePic.cpp rijndael.cpp md5.cpp preferences.cpp openFile.cpp \
                    fileName.cpp setReminder.cpp cfgFile.cpp
DISTFILES	= AUTHORS COPYING COPYRIGHT INSTALL THANKS TODO ChangeLog make_arm
INCLUDEPATH	+= $(QPEDIR)/include
DESTDIR		= ../bin
DEPENDPATH	+= $(QPEDIR)/include
#DEPENDPATH	+= $(CROSSCOMPILE)/include
#DEPENDPATH	+= $(CROSSCOMPILE)/include/g++-3
LIBS            += -lqpe -lstdc++
INTERFACES	= aboutBase.ui noteNameBase.ui defineEntryBase.ui chooseEntryBase.ui writeNoteBase.ui \
		    noteTypeBase.ui writeDefinedEntryBase.ui searchBase.ui \
		    renameNoteBase.ui setTaskBase.ui setEventBase.ui choosePicBase.ui sortBase.ui \
                    deleteConfirmBase.ui preferencesBase.ui startUpPasswdBase.ui openFileBase.ui fileNameBase.ui \
                    setReminderBase.ui
TARGET		= iqnotes
