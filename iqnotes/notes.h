/*  Iqnotes - Smarty notes
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
#ifndef NOTES_H
#define NOTES_H

#include <qwidget.h>
#include <qlistview.h>
#include <qlist.h>
#include <qdatetime.h>
#include <qvaluelist.h>

#include "qsketch.h"
//#include "sortTasks.h"
#include "sort.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QListView;
class QListViewItem;
class QTextView;
class QMultiLineEdit;
class QTextStream;

#ifdef DESKTOP
class QSplitter;
class QVBoxLayout;
#endif

//class QString;
class QPainter;
class QColorGroup;

class Strokes;
class NoteDetail;

// {{{ class Event

class Event
{
public:
    Event();
    Event(const Event &e);
    Event(const QDateTime &dt);

    bool hasTime() const;
    QDateTime getDateTime() const;

    bool isValid() const;

    Event &operator=(const Event &e);

private:
    QDateTime dateTime;
};

// }}}

// {{{ class Reminder

class Reminder
{
public:
    Reminder();
    Reminder(const Reminder &e);
    Reminder(const QDateTime &dt, bool alarmStatus = false);

	bool hasAlarm() const;
    bool hasTime() const;
    QDateTime getDateTime() const;

    bool isValid() const;

    Reminder &operator=(const Reminder &e);
    bool operator<=(const QDateTime &dt) const
    {
        if (dateTime.time().isValid())
        {
            return dateTime <= dt;
        }
        
        QDateTime dt1(dateTime);
        dt1.setTime(QTime(0, 0));
        
        return dt1 <= dt;
    }

private:
    QDateTime dateTime;
	bool alarm;
};

// }}}

// {{{ class PropertyData

class PropertyData
{
public:
    PropertyData()
    {
        id = 0;
        data = "";
    }

    PropertyData(const QString &data1, int id1)
    {
#ifdef DEBUG
        qDebug("PropertyData(const QString &data1, int id1)");
        qDebug("id: %d", id1);
#endif

        id = id1;
        data = data1;
    }

    PropertyData(const PropertyData &pd)
    {
        id = pd.id;
        data = pd.data;
    }

    bool isValid()
    {
        return id != 0;
    }

    int getID() const
    {
        return id;
    }
    const QString &getData() const
    {
        return data;
    }
    void setID(int id1)
    {
        id = id1;
    }
    void setData(const QString& data1)
    {
        data = data1;
    }

    PropertyData &operator=(const PropertyData &pd)
    {
        setID(pd.getID());
        setData(pd.getData());

        return *this;
    }

private:
    int id;
    QString data;
}
;

// }}}

// {{{ class NoteData

// a lot of things here I don't like, needs redesign
class NoteData
{
public:
    NoteData();
    NoteData(NoteData &nd);
    ~NoteData();

    NoteData &operator=(NoteData &nd);
    
    void setDataExternal(bool external = true) { dataExternal = external; }

    bool searchFor(const QString &pattern);

    QString getCompareValue(SortControl sc);
    int compare(NoteData *nd, SortControl sc);

    void deleteEntryProperties(QValueList<int> *deletedProperties);

    // set & get
    unsigned int getID() const { return id; }
    
    void setTitle(const QString &t);
    void setPicFile(const QString &picFileN);

    void setData(QList<QString> *data1);
    void setData(QList<PropertyData> *data1);
    void setTextData(const QString &textData);
    void setPropertyData(const PropertyData &pd);
    void setPropertyData(const QString &propData, int id);
    void setEntryName(const QString &en);
    void setStrokes(Strokes *strokes1);

    void setPercentage(const QString &per);
    void setPriority(int prio) { priority = prio; }

    void setEvent(Event e)
    {
        if (e.getDateTime().date().isValid())
        {
            *percentage = "";
            event = e;
            _isTask = false;
        }
    }
    
    void setReminder(Reminder r)
    {
        if (r.getDateTime().date().isValid())
            reminder = r;
    }

    void unsetTask() { _isTask = false; }
    
    void unsetEvent() { event = Event(QDateTime(QDate(2002, 13, 33), QTime(25, 61))); }
    
    void unsetReminder() { reminder = Reminder(QDateTime(QDate(2002, 13, 33), QTime(25, 61))); }

    QString getEventText();
    const QString &getPicFile() const { return picFile; }

    const QString &getTextData() const;
    
    QList<PropertyData> *getDataPtr() { return data; }
    const QList<PropertyData> &getData() const { return *data; }
    
    PropertyData *getPropertyData(int id1);
    const QString &getTitle() const { return *title; }
    const QString &getEntryName() const { return *entryName; }
    Strokes *getStrokes() { return strokes; }

    const QString &getPercentage() const { return *percentage; }
    int getPercentageInt() { return percentage->toInt(); }
    int getPriority() const { return priority; }

    Event getEvent() { return event; }
    
    Reminder getReminder() { return reminder; }

    // is
    bool isText() const { return (getEntryName() == "" && !isSketch()); }
    bool isFormatedText() const { return (getEntryName() != "" && !isSketch()); }
    bool isTask() const { return _isTask; }
    bool isSketch() const { return strokes != 0; }
    bool isEvent() const { return event.isValid(); }
    bool isOther() const { return (!isTask() && !isEvent()); }
    bool hasPic() const { return picFile.length() > 0; }
    bool hasReminder() const { return reminder.isValid(); }

	
    int getNoteType() const
    {
        if (isTask())
            return TASK;
        if (isEvent())
            return EVENT;
        return OTHER;
	}

    static unsigned int lastID;

private:
    void init();
    
    unsigned int id;

    Strokes *strokes;
    QList<PropertyData> *data;
    QString *title, *entryName, *percentage, months[12], picFile;
    int priority;
    bool _isTask;
    Event event;
    Reminder reminder;

    bool dataExternal;
};

// }}}

// {{{ class NotesViewItem

class NotesViewItem : public QListViewItem
{
public:
    NotesViewItem(QListView *parent);
    NotesViewItem(NotesViewItem *parent);
    NotesViewItem(QListView *parent, NotesViewItem *after);
    NotesViewItem(NotesViewItem *parent, NotesViewItem *after);
    ~NotesViewItem();

    void setMyPixmap(const QString &picFile);

    void refreshItem();

    // shallow copy of the NoteData ...
    NotesViewItem &operator=(NotesViewItem &nvi);

    void deleteEntryProperties(QValueList<int> *deletedProperties)
    {
        noteData->deleteEntryProperties(deletedProperties);
    }

    virtual void setText(int c, const QString &t)
    {
        QListViewItem::setText(c, t);
        noteData->setTitle(t);
    }
    virtual QString text(int c) const;

    virtual int width(const QFontMetrics& fm, const QListView* lv, int c) const;
    virtual void paintCell(QPainter *, const QColorGroup & cg, int column, int width, int alignment);

    NotesViewItem *firstSibling();
    NotesViewItem *nextSibling()
    {
        return (NotesViewItem *)(QListViewItem::nextSibling());
    }
    NotesViewItem *lastChild();
    NotesViewItem *lastSibling();

    int compare(NotesViewItem *nvi, SortControl sc);

    NoteData *getNoteData()
    {
        return noteData;
    }
    void setNoteData(NoteData *nd, bool external = true);

    Strokes *getStrokes()
    {
        return noteData->getStrokes();
    }
    void setStrokes(Strokes *strokes)
    {
        noteData->setStrokes(strokes);
    }

    void setPercentage(const QString &per)
    { //
        noteData->setPercentage(per);
        // To update width. How else?
        //      QString p = text(0);
        //      setText(0, p + ",");
        //      setText(0, p);
        widthChanged(); // ??
    }
    void setPriority(int prio)
    {
        noteData->setPriority(prio);
        widthChanged();
    }

    void setEvent(Event e)
    {
        noteData->setEvent(e);
        widthChanged();
    }
    
    void setReminder(Reminder r)
    {
        noteData->setReminder(r);
        widthChanged();
    }

    void unsetTask()
    {
        noteData->unsetTask();
        widthChanged();
    }
    void unsetEvent()
    {
        noteData->unsetEvent();
        widthChanged();
    }
    void unsetReminder()
    {
        noteData->unsetReminder();
        widthChanged();
    }

    //    void setPercentage(QString *per) {
    //      percentage = per;
    //      // To update width. How else?
    //      QString p = text(0);
    //      setText(0, p + ",");
    //      setText(0, p);
    //    }

    const QString &getTextData() const
    {
        return noteData->getTextData();
    }
    unsigned int getID() const
    {
        return noteData->getID();
    }
    const QString &getPicFile() const
    {
        return noteData->getPicFile();
    }
    const QString &getPercentage()
    {
        return noteData->getPercentage();
    } //
    int getPercentageInt()
    {
        return noteData->getPercentageInt();
    } //
    int getPriority() const
    {
        return noteData->getPriority();
    }

    Event getEvent() const
    {
        return noteData->getEvent();
    }
    
    Reminder getReminder() const
    {
        return noteData->getReminder();
    }

    bool isTask() const
    {
        return noteData->isTask();
    } //
    bool isSketch() const
    {
        return noteData->isSketch();
    }
    bool isText() const
    {
        return noteData->isText();
    }
    bool isEvent() const
    {
        return noteData->isEvent();
    }
    bool hasPic() const
    {
        return noteData->hasPic();
    }
    bool hasReminder() const
    {
        return noteData->hasReminder();
    }

    // For defined Entry
    void setEntryName(const QString &name)
    {
        noteData->setEntryName(name);
    } //
    const QString &getEntryName()
    {
        return noteData->getEntryName();
    } //
    void setTextData(const QString &textData)
    {
        noteData->setTextData(textData);
    }
    void setPropertyData(const QString &propData, int id); //
    //  void setPropertyData(const QList<PropertyData> &propDataList, bool changeExternalStatus = true, bool external = true); //
    void setData(QList<QString> *data1)
    {
        noteData->setData(data1);
    }
    void setData(QList<PropertyData> *data1)
    {
        noteData->setData(data1);
    }

    // Common
    const QString &getFormatedData(); //
    const QString &getPropertyData(int id); //

    void setMatched(bool bold)
    { //
        if (bold)
        {
            defFont.setBold(bold);
            matched = 1;
        }
        else
            matched = 0;
    }

    bool searchFor(const QString &pattern); //

private:
    void init();

    NoteData *noteData;
    bool dataAreExternal;

    QFont defFont;
    int matched;
};

// }}}

// {{{ class Notes

class Notes : public QWidget
{
    Q_OBJECT

public:
    enum EViewType { NONE, HIDE_TREE, HIDE_NOTE, HALF_VIEW };
    Notes(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~Notes();

	bool quickAdd();
    bool addBefore();
    bool addAfter();
    bool addChild();

    void clearTree();

    void expandTree();
    void collapseTree();
    
    bool setReminder();
    bool unsetReminder();

    void taskListWholeTree();
    void taskListCurrent();

    void eventListWholeTree();
    void eventListCurrent();
    
    void showReminder(bool showDialogIfNone = true);

    bool deleteNote();
    bool copyNoteOnlyCurrent();
    bool copyNoteCurrentAndDown();
private:
    bool copyNoteCurrentAndDownRecur(NotesViewItem *originalNote, NotesViewItem *copyNote);
public:
    bool pasteNoteFirst();
    bool pasteNoteBefore();
    bool pasteNoteAfter();
    bool pasteNoteChild();

    bool isEntryUsed(const QString &entryName);

    void search();

    void sort();

    bool writeNote(NotesViewItem *nvi);
	bool renameNote();
    bool editNote();

    void halfView();
    void fullTreeView();
    void fullNoteView();
    EViewType getViewType() { return viewType; }

    bool setPicture();
    bool setTask();
    bool setEvent();
    bool unsetTaskEvent();

    void saveTree(QTextStream *);

    void setCurrentItem(unsigned int id);

    void deleteEntryProperties(const QString &entryName, QValueList<int> *deletedProperties);
    void refreshCurrentItem() { noteChanged(notesTree->currentItem()); }
    
    bool isClipboardEmpty() const { return clipboardNote == 0 ? true : false; }

protected:
    virtual void paintEvent(QPaintEvent *pe);
    virtual void resizeEvent(QResizeEvent *re);

private:
    void taskList(NotesViewItem *startItem = 0);
    void taskListRecur(NotesViewItem *startItem = 0);
    void eventList(NotesViewItem *startItem = 0);
    void eventListRecur(NotesViewItem *startItem = 0);

    void sort(NotesViewItem *thisLevel, SortControl sc, bool goDown = false);
    void saveTreeRecur(NotesViewItem *nvi, QTextStream *saveData);
    bool searchRecur(NotesViewItem *nvi, NotesViewItem *parentInSearchTree);
    void closeSearchTree();
    void refreshItems();

    QListView *getCurrentTree();

public slots:
    void noteChanged(QListViewItem *lvi);
    bool editNote(QListViewItem *lvi);

    void taskListClose();
    void eventListClose();
    void reminderClose();

    void setWordWrap(bool);
    void setRootNodeDecoration(bool);
    void setShowScrollBars(bool);
    void setVerticalLayout(bool);

    void destroyNotes();
    void createNotes();

    // When filling from scratch!
    void addNote(const QString &, QList<QString> *, const QString &, const QString &, int, QDateTime, const QString &, QDateTime);
    void addNote(const QString &, Strokes *, const QString &, const QString &, int, QDateTime, const QString &, QDateTime);
    void noNoteChild();

signals:
    void emptyNoteTree();
    void noEmptyNoteTree();

    void searchTreeShown();
    void searchTreeClosed();

    void taskListShown();
    void taskListClosed();

    void eventListShown();
    void eventListClosed();
    
    void reminderShown();
    void reminderClosed();

    void noteModified(bool);

private:
    bool notesDestroyed;
    bool addFirst();
    QString getItemName();
    EViewType viewType, setViewType;

    SortControl taskListSC, eventListSC, reminderSC, mainSC;

    bool wordWrap, showScrollBars, verticalLayout;

    NotesViewItem *clipboardNote;

    unsigned int noteDataLastID;

    NotesViewItem *forSearchRecur,
    *currentParent;
    QString searchPattern;

    // Widgets
#ifdef DESKTOP
	QSplitter *splitter;
	QVBoxLayout *vBoxLayout;
	QValueList<int> splitterSize;
#endif
    NoteDetail *noteDetail;
    QListView *notesTree, *searchTree, *taskTree, *eventTree, *reminderTree;
};

// }}}

#endif
