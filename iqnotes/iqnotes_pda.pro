TEMPLATE = app
CONFIG += qtopiaapp
CONFIG -= buildQuicklaunch

# Set this to 1.5 to target the Sharp Zaurus.
# Set this to 1.7 to target the Archos PMA400.
# Set this to 2 to target a Qtopia 2.x device.
# Leave this blank to use highest value the SDK's allows.
# This prevents linking against libraries that aren't
# present in older Qtopia releases. You will need to avoid
# new functions in old classes too. You can target a
# newer version and install on an older device if you also
# copy the extra libraries to the device.
QTOPIA_COMPAT_VER=1.5

#DEFINES = TOOLBAR_BIG_ICONS
TARGET = iqnotes
SOURCES = *.cpp
HEADERS = *.h
DISTFILES  = AUTHORS \
             COPYING \
             COPYRIGHT \
             INSTALL \
             THANKS \
             TODO \
             ChangeLog \
             make_arm 

INTERFACES = \
pda/ui/aboutBase.ui \
pda/ui/chooseEntryBase.ui \
pda/ui/choosePicBase.ui \
pda/ui/defineEntryBase.ui \
pda/ui/deleteConfirmBase.ui \
pda/ui/fileNameBase.ui \
pda/ui/iqnotesAlarmBase.ui \
pda/ui/noteNameBase.ui \
pda/ui/noteTypeBase.ui \
pda/ui/openFileBase.ui \
pda/ui/preferencesBase.ui \
pda/ui/renameNoteBase.ui \
pda/ui/searchBase.ui \
pda/ui/setEventBase.ui \
pda/ui/setReminderBase.ui \
pda/ui/setTaskBase.ui \
pda/ui/sortBase.ui \
pda/ui/sortTasksBase.ui \
pda/ui/startUpPasswdBase.ui \
pda/ui/writeDefinedEntryBase.ui \
pda/ui/writeNoteBase.ui
