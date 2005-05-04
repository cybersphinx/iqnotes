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
#include "app.h"
#include "aboutBase.h"
#include "notes.h"
#include "defineEntry.h"
#include "entry.h"
#include "chooseEntry.h"
#include "notesFile.h"
#include "qsketch.h"
#include "preferences.h"
#include "rijndael.h"
#include "startUpPasswdBase.h"
#include "fileName.h"
#include "openFile.h"
#include "cfgFile.h"
#include "toolBarIcon.h"

#include <qdir.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qtooltip.h>
#include <qaction.h>
#include <qxml.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qvaluelist.h>
#include <qlabel.h>
#include <qcstring.h>
#include <qfont.h>
#include <qcombobox.h>

#include <qpe/qpemenubar.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>
#include <qpe/global.h>

App *IQApp;

App::App(QWidget* parent, const char* name, WFlags fl) : QMainWindow(parent, name, fl)
{
    showFullScreen = false;
    setCaption("IQNotes");

    toolbar = new QPEToolBar(this);
    toolbar->setVerticalStretchable(false);
    toolbar->setHorizontalStretchable(false);
    
    setToolBarsMovable(false);
    multiTB = new QToolButton(toolbar);
    
    // File menu
    filePopupMenu = new QPopupMenu(this);
	menu = new QPEMenuBar(this);

    int fileID;
	fileID = menu->insertItem("File", filePopupMenu);
	
#ifndef DEMO
    newID = filePopupMenu->insertItem("New", this, SLOT(newFile()), SHIFT+Key_N);
    openID = filePopupMenu->insertItem("Open", this, SLOT(openFile()), SHIFT+Key_O);
    saveID = filePopupMenu->insertItem("Save", this, SLOT(saveFile()), SHIFT+Key_S);
    // file->insertItem("Save as", this, SLOT(saveAsFile()));
    closeID = filePopupMenu->insertItem("Close", this, SLOT(closeFileMenu()));
    filePopupMenu->insertSeparator();
#endif
	
    filePopupMenu->insertItem("Quit", this, SLOT(goodBye()));

    // Tree menu
    treePopupMenu = new QPopupMenu(this);
    treeID = menu->insertItem("Tree", treePopupMenu);

    searchA = new QAction("Search", ToolBarIcon::prepare("iqnotes/find"), QString::null, Key_F, this, 0 );
    connect(searchA, SIGNAL(activated()), this, SLOT(search()));
    //  searchA->addTo(toolbar);
    searchA->addTo(treePopupMenu);
    multiTB->setIconSet(QIconSet(ToolBarIcon::prepare("iqnotes/find")));
    connect(multiTB, SIGNAL(clicked()), this, SLOT(search()));
	//    toolbar->addSeparator(); // no room for this

    treePopupMenu->insertSeparator();

    quickAddA = new QAction("Quick add", ToolBarIcon::prepare("iqnotes/quick_add"), QString::null, Key_Q, this, 0 );
    connect(quickAddA, SIGNAL(activated()), this, SLOT(quickAdd()));
    quickAddA->addTo(toolbar);
    quickAddA->addTo(treePopupMenu);

    addBeforeA = new QAction("Add before", ToolBarIcon::prepare("iqnotes/add_before"), QString::null, 0, this, 0 );
    connect(addBeforeA, SIGNAL(activated()), this, SLOT(addBefore()));
    addBeforeA->addTo(toolbar);
    addBeforeA->addTo(treePopupMenu);

    addAfterA = new QAction("Add after", ToolBarIcon::prepare("iqnotes/add_after"), QString::null, Key_A, this, 0 );
    connect(addAfterA, SIGNAL(activated()), this, SLOT(addAfter()));
    addAfterA->addTo(toolbar);
    addAfterA->addTo(treePopupMenu);

    addChildA = new QAction("Add child", ToolBarIcon::prepare("iqnotes/add_child"), QString::null, Key_E, this, 0 );
    connect(addChildA, SIGNAL(activated()), this, SLOT(addChild()));
    addChildA->addTo(toolbar);
    addChildA->addTo(treePopupMenu);

    treePopupMenu->insertSeparator();
    sortID = treePopupMenu->insertItem("Sort", this, SLOT(sort()));
    treePopupMenu->insertSeparator();

    expandTreeID = treePopupMenu->insertItem("Expand tree", this, SLOT(expandTree()));
    collapseTreeID = treePopupMenu->insertItem("Collapse tree", this, SLOT(collapseTree()));

    treePopupMenu->insertSeparator();

    taskListPopupMenu = new QPopupMenu(this);
    taskListPopupMenu->insertItem("From whole tree", this, SLOT(taskListWholeTree()));
    taskListPopupMenu->insertItem("From current note down", this, SLOT(taskListCurrent()));

    eventListPopupMenu = new QPopupMenu(this);
    eventListPopupMenu->insertItem("From whole tree", this, SLOT(eventListWholeTree()));
    eventListPopupMenu->insertItem("From current note down", this, SLOT(eventListCurrent()));

    taskListID = treePopupMenu->insertItem("Task list", taskListPopupMenu);
    eventListID = treePopupMenu->insertItem("Event list", eventListPopupMenu);

    treePopupMenu->insertSeparator();
    reminderID = treePopupMenu->insertItem("Reminder", this, SLOT(showReminder()));

    toolbar->addSeparator();

    /*
    closeSearchTreeA = new QAction("Close search tree", Resource::loadPixmap("iqnotes/close_search_tree"), QString::null, 0, this, 0 );
    connect(closeSearchTreeA, SIGNAL(activated()), this, SLOT(closeSearchTree()));
    closeSearchTreeA->addTo(tree);*/

    // Note menu
    notePopupMenu = new QPopupMenu(this);
    noteID = menu->insertItem("Note", notePopupMenu);

    renameNoteID = notePopupMenu->insertItem("Rename", this, SLOT(renameNote()), Key_R);

    editA = new QAction("Edit", ToolBarIcon::prepare("iqnotes/edit"), QString::null, Key_Return, this, 0 );
    editA->setToolTip("Edit note");
    connect(editA, SIGNAL(activated()), this, SLOT(editNote()));
    editA->addTo(toolbar);
    editA->addTo(notePopupMenu);

    cutA = new QAction("Cut", ToolBarIcon::prepare("iqnotes/bin"), QString::null, Key_X, this, 0 );
    cutA->setToolTip("Cut note");
    connect(cutA, SIGNAL(activated()), this, SLOT(cutNote()));
    cutA->addTo(toolbar);
    cutA->addTo(notePopupMenu);

    copyNotePopupMenu = new QPopupMenu(this);
    copyNotePopupMenu->insertItem("Only current note", this, SLOT(copyNoteOnlyCurrent()));
    copyNotePopupMenu->insertItem("Current note and down", this, SLOT(copyNoteCurrentAndDown()));
    copyNoteID = notePopupMenu->insertItem("Copy", copyNotePopupMenu);

    pasteNotePopupMenu = new QPopupMenu(this);
    pasteNotePopupMenu->insertItem("Before", this, SLOT(pasteNoteBefore()));
    pasteNotePopupMenu->insertItem("After", this, SLOT(pasteNoteAfter()), SHIFT+Key_A);
    pasteNotePopupMenu->insertItem("As child", this, SLOT(pasteNoteChild()), SHIFT+Key_E);
    pasteNoteID = notePopupMenu->insertItem("Paste", pasteNotePopupMenu);

    notePopupMenu->insertSeparator();

    setPictureID = notePopupMenu->insertItem("Set picture", this, SLOT(setPicture()));

    notePopupMenu->insertSeparator();

    setTaskID = notePopupMenu->insertItem("Set task", this, SLOT(setTask()));
    setEventID = notePopupMenu->insertItem("Set event", this, SLOT(setEvent()));
    unsetTaskEventID = notePopupMenu->insertItem("Unset", this, SLOT(unsetTaskEvent()));

    notePopupMenu->insertSeparator();
    setReminderID = notePopupMenu->insertItem("Set reminder", this, SLOT(setReminder()));
    unsetReminderID = notePopupMenu->insertItem("Unset reminder", this, SLOT(unsetReminder()));

    // View menu
    viewPopupMenu = new QPopupMenu(this);
    viewID = menu->insertItem("View", viewPopupMenu);

    toolbar->addSeparator();

    hideNoteA = new QAction("Hide note", ToolBarIcon::prepare("iqnotes/hide_note"), QString::null, Key_1, this, 0 );
    hideNoteA->setToolTip("Hide note");
    connect(hideNoteA, SIGNAL(activated()), this, SLOT(hideNote()));
    hideNoteA->addTo(toolbar);
    hideNoteA->addTo(viewPopupMenu);

    hideTreeA = new QAction("Hide tree", ToolBarIcon::prepare("iqnotes/hide_tree"), QString::null, Key_2, this, 0 );
    hideTreeA->setToolTip("Hide tree");
    connect(hideTreeA, SIGNAL(activated()), this, SLOT(hideTree()));
    hideTreeA->addTo(toolbar);
    hideTreeA->addTo(viewPopupMenu);

    halfViewA = new QAction("Half view", ToolBarIcon::prepare("iqnotes/half_view"), QString::null, Key_3, this, 0 );
    halfViewA->setToolTip("Half view");
    connect(halfViewA, SIGNAL(activated()), this, SLOT(halfView()));
    halfViewA->addTo(toolbar);
    halfViewA->addTo(viewPopupMenu);

	/*
    viewPopupMenu->insertSeparator();
    toggleToolBarID = viewPopupMenu->insertItem("Toggle toolbar", this, SLOT(toggleToolBar()));
    toggleFullScreenID = viewPopupMenu->insertItem("Toggle fullscreen", this, SLOT(toggleFullScreen()));
    */
	
    // Options menu
    optionsPopupMenu = new QPopupMenu(this);
    optionsID = menu->insertItem("Options", optionsPopupMenu);

    optionsPopupMenu->insertItem("Define new entry", this, SLOT(defineNewEntry()));
    optionsPopupMenu->insertItem("Change entry", this, SLOT(changeEntry()));
    optionsPopupMenu->insertItem("Delete entry", this, SLOT(deleteEntry()));

    optionsPopupMenu->insertSeparator();

    optionsPopupMenu->insertItem("Preferences", this, SLOT(preferenc()));

    // Help menu
    helpPopupMenu = new QPopupMenu(this);
    menu->insertItem("Help", helpPopupMenu);

    helpPopupMenu->insertItem("About", this, SLOT(about()));

    addToolBar(toolbar);

    IQApp = this;

    //
    notes = new Notes(this, "bla");
    setCentralWidget(notes);

    connect(notes, SIGNAL(emptyNoteTree()), this, SLOT(isEmptyNoteTree()));
    connect(notes, SIGNAL(noEmptyNoteTree()), this, SLOT(isNotEmptyNoteTree()));
    connect(notes, SIGNAL(searchTreeShown()), this, SLOT(searchTreeShown()));
    connect(notes, SIGNAL(searchTreeClosed()), this, SLOT(searchTreeClosed()));
    connect(notes, SIGNAL(taskListShown()), this, SLOT(taskListShown()));
    connect(notes, SIGNAL(taskListClosed()), this, SLOT(taskListClosed()));
    connect(notes, SIGNAL(eventListShown()), this, SLOT(eventListShown()));
    connect(notes, SIGNAL(eventListClosed()), this, SLOT(eventListClosed()));
    connect(notes, SIGNAL(reminderShown()), this, SLOT(reminderShown()));
    connect(notes, SIGNAL(reminderClosed()), this, SLOT(reminderClosed()));

    readConfig();
    changeFont();
    
    noNoteTree();
}

App::~App()
{}

void App::resizeEvent(QResizeEvent *)
{
    if (showFullScreen && (size() != qApp->desktop()->size()))
        setFullScreen (true);
}

void App::focusInEvent (QFocusEvent *)
{
    if (showFullScreen)
        setFullScreen(true);
}

void App::setFullScreen(bool fullScreen)
{
    static QSize normalsize;

    if (fullScreen)
    {
        if (!normalsize.isValid())
            normalsize = size();

        setFixedSize(qApp->desktop()->size());
        showNormal();
        reparent(0, WStyle_Customize | WStyle_NoBorder, QPoint (0, 0));
        QMainWindow::showFullScreen();
    }
    else
    {
        showNormal();
        reparent(0, 0, QPoint (0, 0));
        resize(normalsize);
        showMaximized();
        normalsize = QSize();
    }
}

bool App::readConfig()
{
    QFile cfgFile(Global::applicationFileName("iqnotes", "iqnotes.cfg"));

    if (!cfgFile.open(IO_ReadOnly))
    {
        if (!cfgFile.open(IO_WriteOnly))
            return false;

        return saveConfig();
    }
    cfgFile.close();

    CfgFileParser handler;
    QXmlInputSource *source;
    QXmlSimpleReader reader;

    connect(&handler, SIGNAL(configLoaded(const NotesConfig &)), this, SLOT(configLoaded(const NotesConfig &)));
    source = new QXmlInputSource(cfgFile);
    reader.setContentHandler(&handler);
    //    reader.setErrorHandler(new NotesFileParserError);
    if(!reader.parse(*source))
    {
        qWarning(tr("can't load config file"));
        delete source;

        return false;
    }

    delete source;
    return true;
}

bool App::saveConfig()
{
    QFile cfgFile(Global::applicationFileName("iqnotes", "iqnotes.cfg"));

    if (!cfgFile.open(IO_WriteOnly))
        return false;

    QTextStream cfgText(&cfgFile);
    cfgText << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
            << "<iqnotes>\n"
            << "<config rootnodedecoration=\"" << (config.rootNodeDecoration ? "yes" : "no") << "\" "
            << "showscrollbars=\"" << (config.showScrollBars ? "yes" : "no") << "\" "
            << "wordwrap=\"" << (config.wordWrap ? "yes" : "no") << "\" "
            << "verticalLayout=\"" << (config.verticalLayout ? "yes" : "no") << "\" "
            << "font=\"" << (config.font) << "\" "
            << "fontsize=\"" << (config.fontSize) << "\" "
            << "/>\n"
            << "</iqnotes>\n";

    cfgFile.close();

    return true;
}

void App::configLoaded(const NotesConfig &c)
{
    config = c;
}

void App::closeEvent(QCloseEvent *ce)
{
    if (!closeFile())
        ce->ignore();
    else
        ce->accept();

    return;
}

void App::keyPressEvent(QKeyEvent *ke)
{
    qDebug("Key: %x", ke->key());
    if (ke->key() == Qt::Key_F10 && !noFileOpened) {
        Notes::EViewType viewType = notes->getViewType();

        if (viewType == Notes::HALF_VIEW)
            hideTree();
        else if (viewType == Notes::HIDE_TREE)
            hideNote();
        else if (viewType == Notes::HIDE_NOTE)
            halfView();

        ke->accept();
        return;
    }

    ke->ignore();
}

void App::createNotes()
{
    entriesList = new Entries();

    preferences = Preferences();

    notes->createNotes();

    notes->setWordWrap(config.wordWrap);
    notes->setRootNodeDecoration(config.rootNodeDecoration);
    notes->setShowScrollBars(config.showScrollBars);
    notes->setVerticalLayout(config.verticalLayout);

    isEmptyNoteTree();
    halfView();

    noFileOpened = false;
}

void App::destroyNotes()
{
    notes->destroyNotes();

    if (entriesList)
        delete entriesList;
    entriesList = 0;

    noNoteTree();

    currentFile = "";
    noFileOpened = true;
}

void App::setBackupLocation(bool save, const QString &location)
{
    preferences.saveBackupFile = save;
    preferences.backupLocation = location;
}

void App::setNotesShowReminderAtStartUp(bool showReminder_)
{
    preferences.showReminder = showReminder_;
}

void App::setSecurityPasswd(const QString &passwd)
{
    preferences.passwdHex = passwd;
}

void App::changeFont()
{
    if (config.font == "" || config.fontSize == 0)
        return;

    QFont f(config.font, config.fontSize);

    qApp->setFont(f, true);
    /*
    filePopupMenu->setFont(f);
    notePopupMenu->setFont(f);
    viewPopupMenu->setFont(f);
    optionsPopupMenu->setFont(f);
    helpPopupMenu->setFont(f);
    treePopupMenu->setFont(f);
    taskListPopupMenu->setFont(f);
    eventListPopupMenu->setFont(f);
    copyNotePopupMenu->setFont(f);
    pasteNotePopupMenu->setFont(f);
    */
}

const QString App::quoteXML(const QString &s)
{
    QString r;

    for (uint i = 0; i < s.length(); i++)
    {
        /*
        if (s[i] == '\\')
          r += "\\\\";
          else */
        if (s[i] == '"')
            r += "&quot;";
        else if (s[i] == '>')
            r += "&gt;";
        else if (s[i] == '<')
            r += "&lt;";
        else if (s[i] == '&')
            r += "&amp;";
        else
            r += s[i];
    }

    return r;
}

void App::addEntry(Entry *e)
{
    entriesList->addEntry(e);
}

void App::deleteEntry(const QString &entryName)
{
    Entry *entry;
    uint i = 0;

    if (notes->isEntryUsed(entryName))
        emit canDeleteEntry(entryName, false);
    else
        emit canDeleteEntry(entryName, true);

    for (entry = entriesList->first(); entry; entry = entriesList->next())
    {
        if (entry->getName() == entryName)
            break;

        i++;
    }

    entriesList->remove
    (i);
}

void App::noNoteTree()
{
    noFileOpened = true;
    isEmptyNoteTree();

    menu->setItemEnabled(saveID, false);
    menu->setItemEnabled(closeID, false);
    menu->setItemEnabled(treeID, false);
    menu->setItemEnabled(noteID, false);
    menu->setItemEnabled(viewID, false);
    menu->setItemEnabled(optionsID, false);

    quickAddA->setEnabled(false);
    addBeforeA->setEnabled(false);
    addAfterA->setEnabled(false);
    addChildA->setEnabled(false);

    hideTreeA->setEnabled(false);
    hideNoteA->setEnabled(false);
    halfViewA->setEnabled(false);
}

void App::isEmptyNoteTree()
{
    multiTB->setEnabled(false);
    editA->setEnabled(false);
    cutA->setEnabled(false);
    menu->setItemEnabled(copyNoteID, false);
    if (notes->isClipboardEmpty())
        menu->setItemEnabled(pasteNoteID, false);
    else
        menu->setItemEnabled(pasteNoteID, true);
    
    searchA->setEnabled(false);

    menu->setItemEnabled(sortID, false);
    menu->setItemEnabled(expandTreeID, false);
    menu->setItemEnabled(collapseTreeID, false);
    menu->setItemEnabled(taskListID, false);
    menu->setItemEnabled(eventListID, false);
    menu->setItemEnabled(reminderID, false);

	menu->setItemEnabled(renameNoteID, false);
    menu->setItemEnabled(setPictureID, false);
    menu->setItemEnabled(setTaskID, false);
    menu->setItemEnabled(setEventID, false);
    menu->setItemEnabled(unsetTaskEventID, false);
    menu->setItemEnabled(setReminderID, false);
    menu->setItemEnabled(unsetReminderID, false);

    quickAddA->setEnabled(true);
    addBeforeA->setEnabled(true);
    addAfterA->setEnabled(true);
    addChildA->setEnabled(true);

    menu->setItemEnabled(saveID, true);
    menu->setItemEnabled(closeID, true);
    menu->setItemEnabled(treeID, true);
    menu->setItemEnabled(noteID, true);
    menu->setItemEnabled(viewID, true);
    menu->setItemEnabled(optionsID, true);
}

void App::isNotEmptyNoteTree()
{
    multiTB->setEnabled(true);
    editA->setEnabled(true);
    cutA->setEnabled(true);
    menu->setItemEnabled(copyNoteID, true);
    menu->setItemEnabled(pasteNoteID, true);

    searchA->setEnabled(true);

    menu->setItemEnabled(sortID, true);
    menu->setItemEnabled(expandTreeID, true);
    menu->setItemEnabled(collapseTreeID, true);
    menu->setItemEnabled(taskListID, true);
    menu->setItemEnabled(eventListID, true);
    menu->setItemEnabled(reminderID, true);

	menu->setItemEnabled(renameNoteID, true);
    menu->setItemEnabled(setPictureID, true);
    menu->setItemEnabled(setTaskID, true);
    menu->setItemEnabled(setEventID, true);
    menu->setItemEnabled(unsetTaskEventID, true);
    menu->setItemEnabled(setReminderID, true);
    menu->setItemEnabled(unsetReminderID, true);

    quickAddA->setEnabled(true);
    addBeforeA->setEnabled(true);
    addAfterA->setEnabled(true);
    addChildA->setEnabled(true);

    menu->setItemEnabled(saveID, true);
    menu->setItemEnabled(closeID, true);
    menu->setItemEnabled(treeID, true);
    menu->setItemEnabled(noteID, true);
    menu->setItemEnabled(viewID, true);
    menu->setItemEnabled(optionsID, true);

    /*
    hideTreeA->setEnabled(true);
    hideNoteA->setEnabled(true);
    halfViewA->setEnabled(true);
    */
}

void App::taskListShown()
{
    treePopupMenu->removeItem(taskListID);
    taskListID = treePopupMenu->insertItem(Resource::loadPixmap("iqnotes/close_search_tree"), "Close task list", notes, SLOT(taskListClose()), 0, -1, 11);
    multiTB->disconnect();
    multiTB->setIconSet(Resource::loadPixmap("iqnotes/close_search_tree"));
    connect(multiTB, SIGNAL(clicked()), notes, SLOT(taskListClose()));

    searchA->setEnabled(false);

    quickAddA->setEnabled(false);
    addBeforeA->setEnabled(false);
    addAfterA->setEnabled(false);
    addChildA->setEnabled(false);

    menuBar()->setItemEnabled(expandTreeID, false);
    menu->setItemEnabled(collapseTreeID, false);

    //    menu->setItemEnabled(sortID, false);
    menu->setItemEnabled(eventListID, false);
    menu->setItemEnabled(reminderID, false);

    menu->setItemEnabled(setEventID, false);
    menu->setItemEnabled(unsetTaskEventID, false);

    cutA->setEnabled(false);
    menu->setItemEnabled(copyNoteID, false);
    menu->setItemEnabled(pasteNoteID, false);
}

void App::taskListClosed()
{
    treePopupMenu->removeItem(taskListID);
    taskListID = taskListID = treePopupMenu->insertItem("Task list", taskListPopupMenu, -1, 11);
    multiTB->disconnect();
    multiTB->setIconSet(Resource::loadPixmap("iqnotes/find"));
    connect(multiTB, SIGNAL(clicked()), this, SLOT(search()));

    searchA->setEnabled(true);

    quickAddA->setEnabled(true);
    addBeforeA->setEnabled(true);
    addAfterA->setEnabled(true);
    addChildA->setEnabled(true);

    menu->setItemEnabled(expandTreeID, true);
    menu->setItemEnabled(collapseTreeID, true);

    //    menu->setItemEnabled(sortID, true);
    menu->setItemEnabled(eventListID, true);
    menu->setItemEnabled(reminderID, true);

    menu->setItemEnabled(setEventID, true);
    menu->setItemEnabled(unsetTaskEventID, true);

    cutA->setEnabled(true);
    menu->setItemEnabled(copyNoteID, true);
    menu->setItemEnabled(pasteNoteID, true);
}

void App::eventListShown()
{
    treePopupMenu->removeItem(eventListID);
    eventListID = treePopupMenu->insertItem(Resource::loadPixmap("iqnotes/close_search_tree"), "Close event list", notes, SLOT(eventListClose()), 0, -1, 12);
    multiTB->disconnect();
    multiTB->setIconSet(Resource::loadPixmap("iqnotes/close_search_tree"));
    connect(multiTB, SIGNAL(clicked()), notes, SLOT(eventListClose()));

    searchA->setEnabled(false);

    quickAddA->setEnabled(false);
    addBeforeA->setEnabled(false);
    addAfterA->setEnabled(false);
    addChildA->setEnabled(false);

    menu->setItemEnabled(expandTreeID, false);
    menu->setItemEnabled(collapseTreeID, false);
    menu->setItemEnabled(reminderID, false);

    //    menu->setItemEnabled(sortID, false);
    menu->setItemEnabled(taskListID, false);

    menu->setItemEnabled(setTaskID, false);
    menu->setItemEnabled(unsetTaskEventID, false);

    cutA->setEnabled(false);
    menu->setItemEnabled(copyNoteID, false);
    menu->setItemEnabled(pasteNoteID, false);
}

void App::eventListClosed()
{
    treePopupMenu->removeItem(eventListID);
    eventListID = treePopupMenu->insertItem("Event list", eventListPopupMenu, -1, 12);
    multiTB->disconnect();
    multiTB->setIconSet(Resource::loadPixmap("iqnotes/find"));
    connect(multiTB, SIGNAL(clicked()), this, SLOT(search()));

    searchA->setEnabled(true);

    quickAddA->setEnabled(true);
    addBeforeA->setEnabled(true);
    addAfterA->setEnabled(true);
    addChildA->setEnabled(true);

    menu->setItemEnabled(expandTreeID, true);
    menu->setItemEnabled(collapseTreeID, true);

    //    menu->setItemEnabled(sortID, true);
    menu->setItemEnabled(taskListID, true);
    menu->setItemEnabled(reminderID, true);

    menu->setItemEnabled(setTaskID, true);
    menu->setItemEnabled(unsetTaskEventID, true);

    cutA->setEnabled(true);
    menu->setItemEnabled(copyNoteID, true);
    menu->setItemEnabled(pasteNoteID, true);
}

void App::searchTreeShown()
{
    searchA->setIconSet(Resource::loadPixmap("iqnotes/close_search_tree"));
    searchA->setText(tr("Close search tree"));
    multiTB->setIconSet(Resource::loadPixmap("iqnotes/close_search_tree"));

    quickAddA->setEnabled(false);
    addBeforeA->setEnabled(false);
    addAfterA->setEnabled(false);
    addChildA->setEnabled(false);

    menu->setItemEnabled(taskListID, false);
    menu->setItemEnabled(eventListID, false);
    menu->setItemEnabled(reminderID, false);

    menu->setItemEnabled(sortID, false);

    cutA->setEnabled(false);
    menu->setItemEnabled(copyNoteID, false);
    menu->setItemEnabled(pasteNoteID, false);
}

void App::searchTreeClosed()
{
    searchA->setIconSet(Resource::loadPixmap("iqnotes/find"));
    searchA->setText(tr("Search"));
    multiTB->setIconSet(Resource::loadPixmap("iqnotes/find"));

    quickAddA->setEnabled(true);
    addBeforeA->setEnabled(true);
    addAfterA->setEnabled(true);
    addChildA->setEnabled(true);

    menu->setItemEnabled(taskListID, true);
    menu->setItemEnabled(eventListID, true);
    menu->setItemEnabled(reminderID, true);

    menu->setItemEnabled(sortID, true);

    cutA->setEnabled(true);
    menu->setItemEnabled(copyNoteID, true);
    menu->setItemEnabled(pasteNoteID, true);
}

void App::reminderShown()
{
    treePopupMenu->removeItem(reminderID);
    reminderID = treePopupMenu->insertItem(Resource::loadPixmap("iqnotes/close_search_tree"), "Close reminder", notes, SLOT(reminderClose()));
    multiTB->disconnect();
    multiTB->setIconSet(Resource::loadPixmap("iqnotes/close_search_tree"));
    connect(multiTB, SIGNAL(clicked()), notes, SLOT(reminderClose()));

    searchA->setEnabled(false);

    quickAddA->setEnabled(false);
    addBeforeA->setEnabled(false);
    addAfterA->setEnabled(false);
    addChildA->setEnabled(false);

    menu->setItemEnabled(expandTreeID, false);
    menu->setItemEnabled(collapseTreeID, false);

    //    menu->setItemEnabled(sortID, false);
    menu->setItemEnabled(taskListID, false);
    menu->setItemEnabled(eventListID, false);

    menu->setItemEnabled(unsetReminderID, false);

    cutA->setEnabled(false);
    menu->setItemEnabled(copyNoteID, false);
    menu->setItemEnabled(pasteNoteID, false);
}

void App::reminderClosed()
{
    treePopupMenu->removeItem(reminderID);
    reminderID = treePopupMenu->insertItem("Reminder", this, SLOT(showReminder()));
    multiTB->disconnect();
    multiTB->setIconSet(Resource::loadPixmap("iqnotes/find"));
    connect(multiTB, SIGNAL(clicked()), this, SLOT(search()));

    searchA->setEnabled(true);

    quickAddA->setEnabled(true);
    addBeforeA->setEnabled(true);
    addAfterA->setEnabled(true);
    addChildA->setEnabled(true);

    menu->setItemEnabled(expandTreeID, true);
    menu->setItemEnabled(collapseTreeID, true);

    //    menu->setItemEnabled(sortID, true);
    menu->setItemEnabled(taskListID, true);
    menu->setItemEnabled(eventListID, true);

    menu->setItemEnabled(unsetReminderID, true);

    cutA->setEnabled(true);
    menu->setItemEnabled(copyNoteID, true);
    menu->setItemEnabled(pasteNoteID, true);
}

void App::openTree(int index, const QString &file)
{
    // no file selected
    if (index == -1)
        return;

    // new data file
    if (index == -2)
    {
        newFile();

        return;
    }

    createNotes();
    if (!openTree(file))
    {
        destroyNotes();

        return;
    }
    
    currentFile = file;
    setModified(false);
    
    foreignDeletedNode = !notes->isClipboardEmpty();
    
    if (preferences.showReminder)
        notes->showReminder(false);
	notes->repaint();
}

bool App::openTree(const QString &file)
{
    NotesFileParser handler;

    connect(&handler, SIGNAL(backupLoaded(bool, const QString &)), this, SLOT(setBackupLocation(bool, const QString &)));
    connect(&handler, SIGNAL(notesShowReminderAtStartUpLoaded(bool)), this, SLOT(setNotesShowReminderAtStartUp(bool)));
    connect(&handler, SIGNAL(notesWordWrapLoaded(bool)), this, SLOT(setNotesWordWrap(bool)));
    connect(&handler, SIGNAL(notesRootNodeDecorationLoaded(bool)), this, SLOT(setNotesRootNodeDecoration(bool)));
    connect(&handler, SIGNAL(notesShowScrollBarsLoaded(bool)), this, SLOT(setNotesShowScrollBars(bool)));
    connect(&handler, SIGNAL(entryLoaded(Entry *)), this, SLOT(addEntry(Entry *)));
    connect(&handler, SIGNAL(noteLoaded(const QString &, QList<QString> *, const QString &, const QString &, int, QDateTime, const QString &, QDateTime)), notes, SLOT(addNote(const QString &, QList<QString> *, const QString &, const QString &, int, QDateTime, const QString &, QDateTime)));
    connect(&handler, SIGNAL(securityPasswdLoaded(const QString &)), this, SLOT(setSecurityPasswd(const QString &)));
    connect(&handler, SIGNAL(noteLoaded(const QString &, Strokes *, const QString &, const QString &, int, QDateTime, const QString &, QDateTime)),
            notes, SLOT(addNote(const QString &, Strokes *, const QString &, const QString &, int, QDateTime, const QString &, QDateTime)));
    connect(&handler, SIGNAL(noNoteChild()), notes, SLOT(noNoteChild()));

    QFile xmlFile(Global::applicationFileName("iqnotes", file + ".xml")),
    xmlRijndaelFile(Global::applicationFileName("iqnotes", file + ".rijn"));
    QXmlInputSource *source;

    // Init xml file
    if (!xmlFile.exists() && !xmlRijndaelFile.exists())
    {
        xmlFile.open(IO_WriteOnly);
        QTextStream ts(&xmlFile);

        ts << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<iqnotes>\n<entries>\n"

        << "<entry name=\"Company\">\n"
        << "<property name=\"Name\" type=\"oneLine\"/>\n"
        << "<property name=\"Address\" type=\"multiLine\"/>\n"
        << "<property name=\"Tels\" type=\"oneLine\"/>\n"
        << "<property name=\"Emails\" type=\"oneLine\"/>\n"
        << "<property name=\"Note\" type=\"multiLine\"/>\n"
        << "</entry>\n"

        << "<entry name=\"Person\">\n"
        << "<property name=\"Name\" type=\"oneLine\"/>\n"
        << "<property name=\"Nick\" type=\"oneLine\"/>\n"
        << "<property name=\"Work Address\" type=\"multiLine\"/>\n"
        << "<property name=\"Work Tel\" type=\"oneLine\"/>\n"
        << "<property name=\"Work Emails\" type=\"oneLine\"/>\n"
        << "<property name=\"Home Address\" type=\"multiLine\"/>\n"
        << "<property name=\"Home Tel\" type=\"oneLine\"/>\n"
        << "<property name=\"Emails\" type=\"oneLine\"/>\n"
        << "<property name=\"Note\" type=\"multiLine\"/>\n"
        << "</entry>\n"

        << "<entry name=\"User account\">\n"
        << "<property name=\"Host\" type=\"oneLine\"/>\n"
        << "<property name=\"Username\" type=\"oneLine\"/>\n"
        << "<property name=\"Password\" type=\"oneLine\"/>\n"
        << "<property name=\"Type\" type=\"oneLine\"/>\n"
        << "<property name=\"Note\" type=\"mutliLine\"/>\n"
        << "</entry>\n"

        << "<entry name=\"Credit card\">\n"
        << "<property name=\"Card type\" type=\"oneLine\"/>\n"
        << "<property name=\"Account #\" type=\"oneLine\"/>\n"
        << "<property name=\"Expire\" type=\"oneLine\"/>\n"
        << "<property name=\"PIN\" type=\"oneLine\"/>\n"
        << "<property name=\"Note\" type=\"mutliLine\"/>\n"
        << "</entry>\n"

        << "<entry name=\"WWW Link\">\n"
        << "<property name=\"Title\" type=\"oneLine\"/>\n"
        << "<property name=\"URL\" type=\"oneLine\"/>\n"
        << "<property name=\"Description\" type=\"multiLine\"/>\n"
        << "</entry>\n"

        << "</entries>\n"
        << "<notes>\n</notes>\n</iqnotes>\n";

        xmlFile.close();
        source = new QXmlInputSource(xmlFile);
    }
    else if (xmlRijndaelFile.exists())
    {
        StartUpPasswdBase passwdD(this, 0, true);
        passwdD.adjustSize();
        if (!passwdD.exec())
        {
            return false;
        }
        
        preferences.setPasswd(passwdD.Passwd->text());

        xmlRijndaelFile.open(IO_ReadOnly);
        uint fileSize = xmlRijndaelFile.size();
        unsigned char *rijnData = (unsigned char *) malloc(fileSize),
            *rijnDataDecrypted = (unsigned char *) malloc(fileSize);
        Rijndael rijndael;

        xmlRijndaelFile.readBlock((char *)rijnData, fileSize);
        rijndael.init(Rijndael::CBC, Rijndael::Decrypt, preferences.passwd16Bin, Rijndael::Key16Bytes);
        rijndael.padDecrypt(rijnData, fileSize, rijnDataDecrypted);

		// yeah, stupid
        if (memcmp((void *)"<iqnotes>", (void *)rijnDataDecrypted, 9) != 0 && memcmp((void *)"<?xml version", (void *)rijnDataDecrypted, 13) != 0)
        {
            free(rijnData);
            free(rijnDataDecrypted);

            QMessageBox::critical(this, "Bad password", "Please,\ntype correct password.");

            return false;
        }

        source = new QXmlInputSource();
        //        source->setData(QString((char *)rijnDataDecrypted));
        source->setData(QString::fromUtf8((char *)rijnDataDecrypted));

        free(rijnData);
        free(rijnDataDecrypted);
    }
    else
    {
        source = new QXmlInputSource(xmlFile);
    }

    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(new NotesFileParserError);
    if(!reader.parse(*source))
        qWarning(tr("parse error"));

    delete source;

    setCaption("IQNotes :: " + file);
     
    return true;
}

bool App::saveTree()
{
    // backup dir checking (see also preferences.cpp)
    if (preferences.saveBackupFile)
    {
        QDir backupDir(preferences.backupLocation);
        
        if (!backupDir.exists())
        {
            QMessageBox::critical(this, "Invalid directory", "Please, enter the existing\ndirectory in backup location.", 0, 0);
            return false;
        }
    }
    
    QFile saveF(Global::applicationFileName("iqnotes", currentFile + ".xml"));
    QFile rijnF(Global::applicationFileName("iqnotes", currentFile + ".rijn"));

    QString saveS;
    QTextStream saveData(saveS, IO_WriteOnly);
	saveData.setEncoding(saveData.UnicodeUTF8);
	
    saveData << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<iqnotes>\n"
    << "<config>\n"
    << "<backup save=\"" << (preferences.saveBackupFile ? "yes" : "no") << "\" location=\"" << preferences.backupLocation << "\"/>\n"
             << "<notes showreminder=\"" << (preferences.showReminder ? "yes" : "no") << "\" />\n"
    << "<security passwd=\"" << preferences.passwdHex << "\"/>\n"
    << "</config>\n"

    << "<entries>\n";

    for (Entry *e = entriesList->first(); e; e = entriesList->next())
    {
        saveData << "<entry name=\"" << quoteXML(e->getName()) << "\" defaultpic=\"" << quoteXML(e->getDefaultPic()) << "\">\n";

        for (PropertyStruct *ps = e->first(); ps; ps = e->next())
        {
            saveData << "<property name=\"" << quoteXML(ps->getName()) << "\" "
            << "type=\"" << (ps->getType() == PropertyStruct::ONE_LINE ? "oneLine" : "multiLine") << "\"/>\n";
        }

        saveData << "</entry>\n";
    }

    saveData << "</entries>\n";

    notes->saveTree(&saveData);

    saveData << "</iqnotes>\n";
	//SF.close();
    QCString dataUTF8 = saveS.utf8();

    if (!preferences.passwdHex.length())
    {
        saveF.open(IO_WriteOnly);

        QTextStream saveData1(&saveF);
		saveData1.setEncoding(saveData1.UnicodeUTF8);
		saveData1 << saveS;

        if (preferences.saveBackupFile)
        {
            QFile backupSaveF(preferences.backupLocation + "/" + currentFile + ".xml");
            QTextStream saveData2(&backupSaveF);
            backupSaveF.open(IO_WriteOnly);
            saveData2 << dataUTF8;
        }

        if (rijnF.exists())
            rijnF.remove();
    }
    else
    {
        Rijndael rijndael;
        unsigned char *rijnEncrypt = (unsigned char*)malloc(dataUTF8.length() + 16);

        rijnF.open(IO_WriteOnly);

        rijndael.init(Rijndael::CBC, Rijndael::Encrypt, preferences.passwd16Bin, Rijndael::Key16Bytes);
        int len = rijndael.padEncrypt((unsigned char*)(const char*)dataUTF8, dataUTF8.length(), rijnEncrypt);

        rijnF.writeBlock((const char *)rijnEncrypt, len);
        if (preferences.saveBackupFile)
        {
            QFile rijnBackupF(preferences.backupLocation + "/" + currentFile + ".rijn");
            rijnBackupF.open(IO_WriteOnly);
            rijnBackupF.writeBlock((const char *)rijnEncrypt, len);
        }

        free(rijnEncrypt);

        if (saveF.exists())
            saveF.remove();
    }

    return true;
}

void App::setModified(bool m)
{
    if (m)
    {
        modified = true;

        menu->setItemEnabled(saveID, true);
        
        if (currentFile.length())
            setCaption("IQNotes :: " + currentFile + "*");
#ifndef DEMO
        else
            setCaption("IQNotes :: (untitled)*");
#endif
    }
    else
    {
        modified = false;

        menu->setItemEnabled(saveID, false);
        
        if (currentFile.length())
            setCaption("IQNotes :: " + currentFile);
#ifndef DEMO
        else
            setCaption("IQNotes :: (untitled)");
#endif
    }
}

// {{{ Methods called from menu

// {{{ File menu

void App::newFile()
{
    if (!closeFile())
        return;

    createNotes();
    if (!openTree("#init_file#"))
    {
        destroyNotes();

        return;
    }

    currentFile = "";
    setModified(false);
#ifndef DEMO
    setCaption("IQNotes :: (untitled)");
#else
	setCaption("IQNotes :: DEMO");
#endif
	
    
    foreignDeletedNode = !notes->isClipboardEmpty();
}

void App::openFile()
{
    openFile(false);
}

void App::openFile(bool showNewFile)
{
    if (!closeFile())
        return;

    OpenFile of(showNewFile, this, 0, true);
    connect(&of, SIGNAL(fileSelected(int, const QString &)), this, SLOT(openTree(int, const QString &)));
    
    of.showMaximized();

    if (!of.exec())
        return;
}

bool App::saveFile()
{
    if (noFileOpened)
        return true;

    if (currentFile == "" && !noFileOpened)
    {
        FileName fn(this, 0, true);

        if (!fn.exec())
            return false;

        currentFile = fn.FileNameI->text();
    }

    if (!saveTree())
        return false;

    setCaption("IQNotes :: " + currentFile);
    setModified(false);

    return true;
}

void App::saveAsFile()
{}

void App::closeFileMenu()
{
    closeFile();
}

bool App::closeFile()
{
#ifdef DEMO
	return true;
#endif
	
    if (noFileOpened)
        return true;

    if (modified)
    {
        int r = QMessageBox::warning(this, "Warning", "Your data is not saved.\n Do you want to save?", QMessageBox::Yes | QMessageBox::Default, QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape);
        
        if (r == QMessageBox::Yes)
        {
            if (!saveFile())
                return false;
        }
        
        if (r == QMessageBox::Cancel)
            return false;
    }

    destroyNotes();

    currentFile = "";
    noFileOpened = true;

    setCaption("IQNotes");

    return true;
}

void App::goodBye()
{
    if (!closeFile())
        return;

    close();
}

// }}}

// {{{ Tree menu

void App::search()
{
    notes->search();
}

bool App::foreignNodeWarning()
{
    if (!foreignDeletedNode)
        return true;

    int r = QMessageBox::warning(this, "Foreign note", "You are adding note from another\ndata file. This is potentially danger\naction.\nPlease, see help for more info.\nDo you want to proceed?", QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape);
    
    if (r == QMessageBox::Yes)
        return true;
    else
        return false;
}

void App::quickAdd()
{
	if (notes->quickAdd())
		setModified(true);
}

void App::addBefore()
{
    if (notes->addBefore())
        setModified(true);
}

void App::addAfter()
{
    if (notes->addAfter())
        setModified(true);
}

void App::addChild()
{
    if (notes->addChild())
        setModified(true);
}

void App::sort()
{
    notes->sort();
    setModified(true);
}

void App::expandTree()
{
    notes->expandTree();
}

void App::collapseTree()
{
    notes->collapseTree();
}

void App::taskListWholeTree()
{
    notes->taskListWholeTree();
}

void App::taskListCurrent()
{
    notes->taskListCurrent();
}

void App::eventListWholeTree()
{
    notes->eventListWholeTree();
}

void App::eventListCurrent()
{
    notes->eventListCurrent();
}

void App::showReminder()
{
    notes->showReminder();
}

// }}}

// {{{ Note menu

void App::cutNote()
{
    if (notes->deleteNote())
        setModified(true);
}

void App::copyNoteOnlyCurrent()
{
    notes->copyNoteOnlyCurrent();
}

void App::copyNoteCurrentAndDown()
{
    notes->copyNoteCurrentAndDown();
}

void App::pasteNoteBefore()
{
    if (notes->pasteNoteBefore())
        setModified(true);
}

void App::pasteNoteAfter()
{
    if (notes->pasteNoteAfter())
        setModified(true);
}

void App::pasteNoteChild()
{
    if (notes->pasteNoteChild())
        setModified(true);
}

void App::renameNote()
{
	if (notes->renameNote())
		setModified(true);
}

void App::editNote()
{
    if (notes->editNote())
        setModified(true);
}

void App::setPicture()
{
    if (notes->setPicture())
        setModified(true);
}

void App::setTask()
{
    if (notes->setTask())
        setModified(true);
}

void App::setEvent()
{
    if (notes->setEvent())
        setModified(true);
}

void App::unsetTaskEvent()
{
    if (notes->unsetTaskEvent())
        setModified(true);
}

void App::setReminder()
{
    if (notes->setReminder())
        setModified(true);
}

void App::unsetReminder()
{
    if (notes->unsetReminder())
        setModified(true);
}

// }}}

// {{{ View menu

void App::hideTree()
{
    notes->fullNoteView();

    hideTreeA->setEnabled(false);
    hideNoteA->setEnabled(true);
    halfViewA->setEnabled(true);
}

void App::hideNote()
{
    notes->fullTreeView();

    hideTreeA->setEnabled(true);
    hideNoteA->setEnabled(false);
    halfViewA->setEnabled(true);
}

void App::halfView()
{
    notes->halfView();

    hideTreeA->setEnabled(true);
    hideNoteA->setEnabled(true);
    halfViewA->setEnabled(false);
}

void App::toggleToolBar()
{
    if (toolbar->isVisible())
        toolbar->hide();
    else
        toolbar->show();
}

void App::toggleFullScreen()
{
    showFullScreen = !showFullScreen;
    setFullScreen (showFullScreen);
}       

// }}}

// {{{ Options menu

void App::defineNewEntry()
{
    DefineEntry de(this, 0, true);
    de.showMaximized();
    if (de.exec())
        setModified(true);
}

void App::changeEntry()
{
    ChooseEntry ce(this, 0, true);
    Entry *entry;

    ce.fillList(entriesList);
    if (!ce.exec())
        return;

    if (ce.currentEntry() == -1)
        return;

    entry = entriesList->at(ce.currentEntry());

    DefineEntry de(this, 0, true);

    if (notes->isEntryUsed(entry->getName()))
    {
        QMessageBox::warning(this, tr("Warning"), tr("This entry is used in notes.\nChanges made to this entry will\naffect all nodes of this type."), 0, 0, 0);

        de.setIsUsed(true);
    }

    for (PropertyStruct *ps = entry->first(); ps; ps = entry->next())
    {
        de.addProperty(new PropertyBoxItem(ps));
    }

    de.EntryName->setText(entry->getName());
    de.setEditEntry(entry);
    de.showMaximized();
	
#ifdef DEBUG
    qDebug("-- Change entry --");
#endif

    int r = de.exec();
    if (r && de.getIsUsed())
    {
        notes->deleteEntryProperties(entry->getName(), de.getDeletedProperties());
    }

    if (r && de.getIsUsed())
        notes->refreshCurrentItem();

    if (r)
        setModified(true);
}

void App::deleteEntry()
{
    ChooseEntry ce(this, 0, true);
    Entry *entry;

    ce.fillList(entriesList);
    if (!ce.exec())
        return;

    if (ce.currentEntry() == -1)
        return;

    entry = entriesList->at(ce.currentEntry());

    if (notes->isEntryUsed(entry->getName()))
    {
        QMessageBox::warning(this, tr("Warning"), tr("This entry is used in notes.\nIt can't be deleted."), 0, 0, 0);

        return;
    }
    
    entriesList->remove
    (entry->getName());

    setModified(true);
}

void App::preferenc()
{
    PreferencesD pb(this, 0, true);

    pb.ShowReminder->setChecked(preferences.showReminder);
    pb.WordWrap->setChecked(config.wordWrap);
    pb.RootNodeDecoration->setChecked(config.rootNodeDecoration);
    pb.ShowScrollBars->setChecked(config.showScrollBars);
    pb.VerticalLayout->setChecked(config.verticalLayout);

    pb.SaveBackup->setChecked(preferences.saveBackupFile);
    pb.BackupLocation->setText(preferences.backupLocation);

    // passwd
    if (!preferences.passwdHex.length())
    {
        pb.CurrentPasswd->setEnabled(false);
        pb.CurrentPasswdLabel->setEnabled(false);
        pb.DontEncrypt->setEnabled(false);
    }

    pb.setCurrentPasswd(preferences.passwdHex);

    pb.showMaximized();
    if (!pb.exec())
        return;

    if (pb.DontEncrypt->isChecked())
    {
        preferences.setPasswd("");
    }
    else if (pb.Passwd1->text().length())
    {
        preferences.setPasswd(pb.Passwd1->text());
    }

    preferences.showReminder = pb.ShowReminder->isChecked();
    config.wordWrap = pb.WordWrap->isChecked();
    config.rootNodeDecoration = pb.RootNodeDecoration->isChecked();
    config.showScrollBars = pb.ShowScrollBars->isChecked();
    config.verticalLayout = pb.VerticalLayout->isChecked();

    // font changed
    if (config.font != pb.Font->currentText() || config.fontSize != pb.FontSize->currentText().toInt())
    {
        config.font = pb.Font->currentText();
        config.fontSize = pb.FontSize->currentText().toInt();

        changeFont();
    }

    preferences.saveBackupFile = pb.SaveBackup->isChecked();
    preferences.backupLocation = pb.BackupLocation->text();
    notes->setWordWrap(config.wordWrap);
    notes->setRootNodeDecoration(config.rootNodeDecoration);
    notes->setShowScrollBars(config.showScrollBars);
    notes->setVerticalLayout(config.verticalLayout);

    saveConfig();

    setModified(true);
}

// }}}

// {{{ Help menu

void App::about()
{
    AboutForm aboutD(this, tr("About Dialog"), true);

    // aboutD.showMaximized();
    aboutD.adjustSize();
    aboutD.exec();
}

// }}}

// }}}
