/*  IQNotes - Smarty notes
    Copyright (C) 2001 Peter Vrabel <kybu@kybu.org>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/
#ifndef APP_H
#define APP_H

#include <stdlib.h>

#include <qmainwindow.h>
#include "entry.h"
#include "md5.h"
#include "notesConfig.h"

class QWidget;
class Notes;
class Entries;
class QAction;
class QPopupMenu;
class QToolButton;
class FileSelector;
class DocLnk;
class QPEToolBar;
class QMenuBar;

class Preferences
{
public:
    Preferences()
    {
        showReminder = true;
        saveBackupFile = false;

        backupLocation = "";
        passwdHex = "";

        passwd16Bin = (unsigned char *)malloc(16);\
        memset(passwd16Bin, 0, 16);
    }

    void setPasswd(const QString &passwd)
    {
        if (passwd.length())
        {
            MD5 md5passwd(passwd);
            passwdHex = md5passwd.hex_digest();

            free(passwd16Bin);
            passwd16Bin = md5passwd.raw_digest();
        }
        else
        {
            passwdHex = "";
            memset(passwd16Bin, 0, 16);
        }
    }

    Preferences &operator=(const Preferences &p)
    {
        showReminder = p.showReminder;
        saveBackupFile = p.saveBackupFile;

        backupLocation = p.backupLocation;
        passwdHex = p.passwdHex;

        memcpy((void *)passwd16Bin, (void *)p.passwd16Bin, 16);

        return *this;
    }

    bool saveBackupFile, showReminder;
    QString backupLocation, passwdHex;
    unsigned char *passwd16Bin;
};

class App : public QMainWindow
{
    Q_OBJECT
public:
    App(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~App();

    static const QString quoteXML(const QString &);

    Entries *entries()
    {
        return entriesList;
    }


    bool foreignNodeWarning();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void focusInEvent(QFocusEvent *);
    virtual void closeEvent(QCloseEvent *ce);
    virtual void keyPressEvent(QKeyEvent *ke);
            
public slots:
    void configLoaded(const NotesConfig &);

    void addEntry(Entry *);
    void deleteEntry(const QString &);

    void setBackupLocation(bool, const QString &);
    void setNotesShowReminderAtStartUp(bool);
    void setSecurityPasswd(const QString &);
    void changeFont();

    void openTree(int index, const QString &file);
    bool openTree(const QString &file);
    bool saveTree();
    bool closeFile();

    void setModified(bool m);

    void createNotes();
    void destroyNotes();

    void noNoteTree();
    void isEmptyNoteTree();
    void isNotEmptyNoteTree();

    void searchTreeShown();
    void searchTreeClosed();

    void taskListShown();
    void taskListClosed();

    void eventListShown();
    void eventListClosed();
    
    void reminderShown();
    void reminderClosed();

    // File menu
    void newFile();
    void openFile();
    void openFile(bool showNewFile);
    bool saveFile();
    void closeFileMenu();
    void saveAsFile();
    void goodBye();
    // Tree menu
    void search();
    void quickAdd();
    void addBefore();
    void addAfter();
    void addChild();
    void sort();
    void expandTree();
    void collapseTree();
    void taskListWholeTree();
    void taskListCurrent();
    void eventListWholeTree();
    void eventListCurrent();
    void showReminder();
    // Note menu
	void renameNote();
    void editNote();
    void cutNote();
    void copyNoteOnlyCurrent();
    void copyNoteCurrentAndDown();
    void pasteNoteBefore();
    void pasteNoteAfter();
    void pasteNoteChild();
    void setPicture();
    void setTask();
    void setEvent();
    void unsetTaskEvent();
    void setReminder();
    void unsetReminder();
    // View menu
    void hideTree();
    void hideNote();
    void halfView();
    void toggleToolBar();
    void toggleFullScreen();
    // Options menu
    void defineNewEntry();
    void changeEntry();
    void deleteEntry();
    void preferenc();
    //Help menu
    void about();

signals:
    void canDeleteEntry(const QString &, bool);

private:
    bool readConfig();
    bool saveConfig();

    void setFullScreen(bool fullScreen);
    
    Notes *notes;
    QPEToolBar *toolbar;
	QMenuBar *menu;
    QPopupMenu *filePopupMenu, *notePopupMenu, *viewPopupMenu, *optionsPopupMenu, *helpPopupMenu, *treePopupMenu, *taskListPopupMenu, *eventListPopupMenu, *copyNotePopupMenu, *pasteNotePopupMenu;
    int newID, openID, saveID, closeID, sortID, expandTreeID, collapseTreeID, reminderID, copyNoteID, pasteNoteID, setPictureID, setTaskID, setEventID, unsetTaskEventID, setReminderID, unsetReminderID, taskListID, eventListID, renameNoteID, noteID, treeID, viewID, toggleToolBarID, toggleFullScreenID, optionsID;
    QAction *editA, *cutA,
    *searchA, *quickAddA, *addBeforeA, *addAfterA, *addChildA,
    *setPictureA, *setTaskA, *setEventA,
    *hideNoteA, *hideTreeA, *halfViewA;
    QToolButton *multiTB; /* Search, Close search tree, close task|event tree */
    Entries *entriesList;
    FileSelector *fileSelector;
    DocLnk *doc;

    QString currentFile;

    Preferences preferences;
    NotesConfig config;
    bool noFileOpened, modified, foreignDeletedNode, showFullScreen;
};

#endif

/*
  Local Variables:
  mode: c++
  End:
*/
  
