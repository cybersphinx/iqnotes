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
#include "notes.h"
#include "app.h"
#include "entry.h"

// Dialogs
#include "noteType.h"
#include "noteNameBase.h"
#include "writeNoteBase.h"
#include "writeSketchNote.h"
#include "writeDefinedEntry.h"
#include "searchBase.h"
#include "choosePic.h"
//#include "setPercentage.h"
#include "setTask.h"
#include "setEvent.h"
#include "setReminder.h"
#include "renameNoteBase.h"
//#include "sortTasks.h"
#include "sort.h"
#include "deleteConfirmBase.h"

#include <qapplication.h>
#include <qlistview.h>
#include <qtextview.h>
#include <qmultilineedit.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qmultilineedit.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qtextstream.h>
#include <qradiobutton.h>
#include <qdatetime.h>
#include <qcombobox.h>
#include <qtabwidget.h>
#include <qscrollview.h>
#include <qregexp.h>

#ifdef DESKTOP
#include <qsplitter.h>
#include <qlayout.h>
#endif

//#include <qpe/alarmserver.h>
#include <qpe/resource.h>

#include <stdio.h>

extern App *IQApp;

unsigned int NoteData::lastID = 0;

// {{{ class Event

Event::Event()
{
}

Event::Event(const Event &e)
{
    *this = e;
}

Event::Event(const QDateTime &dt)
{
    dateTime = dt;
}

bool Event::isValid() const
{
    return dateTime.date().isValid();
}

bool Event::hasTime() const
{
    return dateTime.time().isValid();
}

QDateTime Event::getDateTime() const
{
    return dateTime;
}

Event &Event::operator=(const Event &e)
{
    dateTime = e.getDateTime();

    return *this;
}

// }}}

// {{{ class Reminder

Reminder::Reminder()
{
	alarm = false;
}

Reminder::Reminder(const Reminder &e)
{
    *this = e;
}

Reminder::Reminder(const QDateTime &dt, bool alarmStatus)
{
    dateTime = dt;
	alarm = alarmStatus;
}

bool Reminder::isValid() const
{
    return dateTime.date().isValid();
}

bool Reminder::hasAlarm() const
{
	return alarm;
}

bool Reminder::hasTime() const
{
    return dateTime.time().isValid();
}

QDateTime Reminder::getDateTime() const
{
    return dateTime;
}

Reminder &Reminder::operator=(const Reminder &e)
{
    dateTime = e.getDateTime();
	alarm = e.hasAlarm();

    return *this;
}

// }}}

// {{{ class NoteData

// {{{ Constructor & destructor

NoteData::NoteData()
{
    init();
}

NoteData::NoteData(NoteData &nd)
{
    init();

    *this = nd;
}

void NoteData::init()
{
    id = ++lastID;

    strokes = 0;
    data = new QList<PropertyData>;
    data->setAutoDelete(true);
    entryName = new QString("");
    percentage = new QString("");
    title = new QString("");
    priority = 0;
    _isTask = false;

    setDataExternal(false);

    months[0] = "Jan";
    months[1] = "Feb";
    months[2] = "Mar";
    months[3] = "Apr";
    months[4] = "May";
    months[5] = "Jun";
    months[6] = "Jul";
    months[7] = "Aug";
    months[8] = "Sep";
    months[9] = "Oct";
    months[10] = "Nov";
    months[11] = "Dec";

    picFile = "";
}

NoteData::~NoteData()
{
    if (!dataExternal)
    {
        delete data;
        delete entryName;
        delete percentage;
        delete title;

        if (strokes)
            delete strokes;
    }
}

// }}}

// {{{ Copy things

NoteData &NoteData::operator=(NoteData &nd)
{
    // data
    data->clear();
    
    QListIterator<PropertyData> it(nd.getData());
    for (; it.current(); ++it)
        data->append(new PropertyData(*(it.current())));

    setTitle(nd.getTitle());
    setEntryName(nd.getEntryName());
    setPicFile(nd.getPicFile());

    if (nd.isTask())
    {
        setPercentage(nd.getPercentage());
        setPriority(nd.getPriority());
    }

    if (nd.isEvent())
        setEvent(nd.getEvent());

    if (nd.hasReminder())
        setReminder(nd.getReminder());
    
    if (nd.isSketch())
        setStrokes(new Strokes(*(nd.getStrokes())));

    
    return *this;
}

// }}}

// lower value goes up
QString NoteData::getCompareValue(SortControl sc)
{
    // if i decode this after month pass, i am a boss :>

    QString p;
    if (isTask())
    {
        if (sc.getTasksOrder() == SortControl::ASCEND)
            p.sprintf("%03d", sc.getTasksSortBy() == SortControl::PERCENTAGE ? getPercentageInt() : 5 - priority);
        else
            p.sprintf("%03d", sc.getTasksSortBy() == SortControl::PERCENTAGE ? 101 - getPercentageInt() : priority);

        return p;
    }

    if (isEvent())
    {
        QDateTime dt = event.getDateTime();

        if (event.hasTime())
        {
            if (sc.getEventsOrder() == SortControl::ASCEND)
                p.sprintf("%d%02d%02d%02d%02d", dt.date().year(), dt.date().month(), dt.date().day(),
                          dt.time().hour() + 1, dt.time().minute() + 1);
            else
                p.sprintf("%d%02d%02d%02d%02d", 3000 - dt.date().year(), 12 - dt.date().month(), 31 - dt.date().day(),
                          25 - (dt.time().hour() + 1), 61 - (dt.time().minute() + 1));
        }
        else
        {
            if (sc.getEventsWithoutTime() == SortControl::FIRST)
            {
                if (sc.getEventsOrder() == SortControl::ASCEND)
                    p.sprintf("%d%02d%02d%02d%02d", dt.date().year(), dt.date().month(), dt.date().day(),
                              0, 0);
                else
                    p.sprintf("%d%02d%02d%02d%02d", 3000 - dt.date().year(), 12 - dt.date().month(), 31 - dt.date().day(),
                              0, 0);
            }
            else
            {
                if (sc.getEventsOrder() == SortControl::ASCEND)
                    p.sprintf("%d%02d%02d%02d%02d", dt.date().year(), dt.date().month(), dt.date().day(),
                              25, 66);
                else
                    p.sprintf("%d%02d%02d%02d%02d", 3000 - dt.date().year(), 12 - dt.date().month(), 31 - dt.date().day(),
                              25, 66);
            }
        }

        return p;
    }

    return getTitle();
}

int NoteData::compare(NoteData *nd, SortControl sc)
{
    int nt1 = getNoteType(),
              nt2 = nd->getNoteType();

    if (nt1 == nt2)
    {
        QString v1 = getCompareValue(sc);
        QString v2 = nd->getCompareValue(sc);

        if (nt1 == OTHER)
        {
            if (sc.getOthersOrder() == SortControl::DESCEND)
            {
                if (v1 < v2)
                    return 1;
                else if (v1 > v2)
                    return -1;
                else
                    return 0;
            }
            else if (sc.getOthersOrder() == SortControl::DONT_TOUCH)
            {
                return 0;
            }
        }

        if (v1 < v2)
            return -1;
        else if (v1 > v2)
            return 1;
        else
            return 0;
    }

    if (nt1 == sc.first())
        return -1;
    if (nt1 == sc.second())
    {
        if (nt2 == sc.first())
            return 1;
        else
            return -1;
    }

    return 1;
}

void NoteData::deleteEntryProperties(QValueList<int> *deletedProperties)
{
    QListIterator<PropertyData> it(*data);

    for (; *it; ++it)
    {
        for (uint i = 0; i < deletedProperties->count(); i++)
        {
            if ((*it)->getID() == (*deletedProperties)[i])
            {
                data->removeRef(*it);
            }
        }
    }
}

bool NoteData::searchFor(const QString &pattern)
{
    if (getTitle().find(pattern, 0, false) > -1)
        return true;

    for (uint i = 0; i < data->count(); i++)
    {
        if (data->at(i)->getData().find(pattern, 0, false) > -1)
        {
            return true;
        }
    }

    return false;
}

// {{{ Set & Get things :)

void NoteData::setTitle(const QString &t)
{
    *title = t;
}

void NoteData::setPicFile(const QString &picFileN)
{
    picFile = picFileN;
}

void NoteData::setStrokes(Strokes *strokes1)
{
    if (strokes)
        delete strokes;

    strokes = strokes1;
}

void NoteData::setData(QList<QString> *data1)
{
    QListIterator<QString> lit1(*data1);
    if (!data->count())
    {
        for (int i = 1; lit1.current(); ++lit1, i++)
        {
            data->append(new PropertyData(**lit1, i));
        }
    }
    else
    {
        QListIterator<PropertyData> lit(*data);
        while (*lit && *lit1)
        {
            (*lit)->setData(*(*lit1));
            ++lit;
            ++lit1;
        }
    }
}

void NoteData::setData(QList<PropertyData> *data1)
{
    QListIterator<PropertyData> lit1(*data1);
    for (int i = 1; lit1.current(); ++lit1, i++)
    {
        setPropertyData(*(lit1.current()));
    }
}


void NoteData::setPropertyData(const QString &propData, int id)
{
    PropertyData pd(propData, id);
    setPropertyData(pd);
}

void NoteData::setPropertyData(const PropertyData &pd)
{
    PropertyData *pd1 = getPropertyData(pd.getID());

    *pd1 = pd;
}

void NoteData::setTextData(const QString &textData)
{
    PropertyData pd(textData, 1);
    setPropertyData(pd);
}

void NoteData::setEntryName(const QString &en)
{
    *entryName = en;
}

void NoteData::setPercentage(const QString &per)
{
    if (per.length())
    {
        _isTask = true;
        *percentage = per;
        event = Event();
    }
}

PropertyData *NoteData::getPropertyData(int id1)
{
    QListIterator<PropertyData> it(*data);

    for(; it.current(); ++it)
    {
        if ((*it)->getID() == id1)
            return *it;
    }

    // lazy
#ifdef DEBUG
    qDebug("PropertyData *NoteData::getPropertyData(int id1)");
    qDebug("id: %d", id1);
#endif

    PropertyData *pd = new PropertyData("", id1);
    data->append(pd);

    return pd;
}

const QString &NoteData::getTextData() const
{
    return data->first()->getData();
}

QString NoteData::getEventText()
{
    QString r;
    QDateTime dt = event.getDateTime();

    if (event.hasTime())
        r.sprintf("%02d:%02d %02d %s, %d", dt.time().hour(), dt.time().minute(), dt.date().day(), months[dt.date().month() - 1].latin1(),
                  dt.date().year());
    else
        r.sprintf("%02d %s, %d", dt.date().day(), months[dt.date().month() - 1].latin1(),
                  dt.date().year());

    return r;
}

// }}}

// }}}

// {{{ class NotesViewItem

// {{{ Constructors & destructor

NotesViewItem::NotesViewItem(QListView *parent) : QListViewItem(parent)
{
    init();
}

NotesViewItem::NotesViewItem(NotesViewItem *parent) : QListViewItem(parent)
{
    init();
}

NotesViewItem::NotesViewItem(QListView *parent, NotesViewItem *after) : QListViewItem(parent, after)
{
    init();
}

NotesViewItem::NotesViewItem(NotesViewItem *parent, NotesViewItem *after) : QListViewItem(parent, after)
{
    init();
}

NotesViewItem::~NotesViewItem()
{
    if (!dataAreExternal)
    {
        delete noteData;
    }
}

QString NotesViewItem::text(int c) const
{
    return noteData->getTitle();
}

void NotesViewItem::init()
{
    noteData = new NoteData;
    dataAreExternal = false;

    matched = -1;
}

// }}}

// {{{ Traverse methods

NotesViewItem *NotesViewItem::firstSibling()
{
    QListViewItem *lvi;

    if (!(lvi = parent()))
    {
        lvi = listView()->firstChild();
    }
    else
    {
        lvi = lvi->firstChild();
    }

    return static_cast<NotesViewItem *>(lvi);

}

NotesViewItem *NotesViewItem::lastChild()
{
    NotesViewItem *nvi = static_cast<NotesViewItem *>(firstChild()), *last;
    for (last = nvi; nvi; nvi = nvi->nextSibling())
    {
        last = nvi;
    }

    return last;
}

NotesViewItem *NotesViewItem::lastSibling()
{
    NotesViewItem *nvi = this, *last;
    for (last = nvi; nvi; nvi = nvi->nextSibling())
    {
        last = nvi;
    }

    return last;
}

// }}}

// {{{ Print methods

int NotesViewItem::width( const QFontMetrics& fm, const QListView* lv, int c ) const
{
    // TODO: grrr, rewrite
    QString priorities[5] = { "Very high", "High", "Normal", "Low", "Very low" };

    QFont lvF = lv->font();
    const_cast<QFont &>(defFont).setFamily(lvF.family());
    const_cast<QFont &>(defFont).setPointSize(lvF.pointSize());
    
    QFontMetrics fm1(defFont);
    QString t = text(c);

    QString percentage = noteData->getPercentage();

    if (isTask())
        t += " (" + percentage + "%, " + priorities[getPriority()] + ")";

    if (isEvent())
        t += " (" + noteData->getEventText() + ")";
    
    if (hasReminder())
        t += " ! "; // not exactly, bold

    int w = fm1.width( t ) + lv->itemMargin() * 2;
    //    int w = fm.width( text( c ) ) + lv->itemMargin() * 2;
    const QPixmap * pm = pixmap( c );
    if ( pm )
        w += pm->width() + lv->itemMargin(); // ### correct margin stuff?
    //return QMAX( w, QApplication::globalStrut().width() );
    return w;
}

void NotesViewItem::paintCell ( QPainter *p, const QColorGroup & cg, int column, int width, int alignment )
{
    QColorGroup eventCG(cg), taskCG(cg), reminderCG(cg);
    eventCG.setColor(QColorGroup::Text, QColor(146, 146, 76));
    taskCG.setColor(QColorGroup::Text, QColor(73, 134, 146));
    reminderCG.setColor(QColorGroup::Text, QColor(240, 0, 0));
    QColor titleColor, eventColor, taskColor, reminderColor;

    // TODO: grrr, rewrite
    QString priorities[5] = { "Very high", "High", "Normal", "Low", "Very low" };

    //    int columns = 1;
    int align = alignment;

    QString percentage = noteData->getPercentage();

    if ( !p )
        return;

    QListView *lv = listView();
    
    QFont lvF = lv->font();
    defFont.setFamily(lvF.family());
    defFont.setPointSize(lvF.pointSize());

    QString t = text( column );

    int r = lv ? lv->itemMargin() : 1;
    const QPixmap * icon = pixmap( column );

    p->fillRect( 0, 0, width, height(), cg.brush( QColorGroup::Base ) );

    int marg = lv ? lv->itemMargin() : 1;
    //    bool selected = false;
    //    if ( align != AlignLeft )
    //marg -= lv->d->minRightBearing;
    if ( isSelected() &&
            (column==0 || listView()->allColumnsShowFocus()) )
    {
        p->fillRect( r - marg, 0, width - r + marg, height(),
                     cg.brush( QColorGroup::Highlight ) );
        titleColor = cg.highlightedText();
        eventColor = eventCG.highlightedText();
        taskColor = taskCG.highlightedText();
        reminderColor = reminderCG.highlightedText();
    }
    else
    {
        titleColor = cg.text();
        eventColor = eventCG.text();
        taskColor = taskCG.text();
        reminderColor = reminderCG.text();
    }

    if ( icon )
    {
        p->drawPixmap( r, (height()-icon->height())/2, *icon );
        r += icon->width() + listView()->itemMargin();
    }

    if ( !t.isEmpty() )
    {
        if ( ! (align & AlignTop | align & AlignBottom) )
            align |= AlignVCenter;
        p->setFont(defFont);

        // note name
        if (matched == 0)
            p->setPen(darkGray);
        else
            p->setPen(titleColor);
        p->drawText(r, 0, width-marg-r, height(), align, t);
        
        QFontMetrics fm(defFont);
        r += fm.width(t);

        if (isTask())
        {
            QString t1 = " (" + percentage + "%, " + priorities[getPriority()] + ")";

            p->setPen(taskColor);    
            p->drawText(r, 0, width - marg - r, height(), align, t1);
            
            r += fm.width(t1);
        }

        if (isEvent())
        {
            QString t1 = " (" + noteData->getEventText() + ")";

            p->setPen(eventColor);
            p->drawText(r, 0, width - marg - r, height(), align, t1);
            
            r += fm.width(t1);
        }

        if (hasReminder())
        {
            QString t1 = " ! ";
            QFont reminderFont = defFont;
            
            reminderFont.setBold(true);
            p->setFont(reminderFont);
            p->setPen(reminderColor);
            p->drawText(r, 0, width - marg - r, height(), align, t1);            
        }
    }
}

// }}}

void NotesViewItem::setMyPixmap(const QString &picFile)
{
    if (!picFile.length())
        setPixmap(0, 0);
    else
        setPixmap(0, Resource::loadPixmap("iqnotes/items/" + picFile));
    noteData->setPicFile(picFile);
}

void NotesViewItem::refreshItem()
{
    setMyPixmap(noteData->getPicFile());
}

NotesViewItem &NotesViewItem::operator=(NotesViewItem &nvi)
{
    setNoteData(nvi.getNoteData());
    setMyPixmap(noteData->getPicFile());

    return *this;
}

void NotesViewItem::setNoteData(NoteData *nd, bool external)
{
    if (noteData && !dataAreExternal)
        delete noteData;

    noteData = nd;
    dataAreExternal = external;
}

int NotesViewItem::compare(NotesViewItem *nvi, SortControl sc)
{
    return noteData->compare(nvi->getNoteData(), sc);
}

bool NotesViewItem::searchFor(const QString &pattern)
{
    return noteData->searchFor(pattern);
}

void NotesViewItem::setPropertyData(const QString &propData, int id)
{
    noteData->setPropertyData(propData, id);
}

const QString &NotesViewItem::getPropertyData(int id)
{
    return noteData->getPropertyData(id)->getData();
}

const QString &NotesViewItem::getFormatedData()
{
#ifdef DEBUG
    qDebug("const QString &NotesViewItem::getFormatedData()");
#endif

    if (!noteData->getEntryName().length())
    {
        return getTextData();
    }

    // Set the entry
    Entries *entries = IQApp->entries();
    Entry *entry = entries->isIn(noteData->getEntryName());

    /*
    for (entry = entries->first(); entry; entry = entries->next())
    {
      if (entry->getName() == noteData->getEntryName())
        break;
    }
    */

    PropertyStruct *ps;
    QString *formatedData = new QString;
    int i = 0;

    for (ps = entry->first(); ps; ps = entry->next())
    {
#ifdef DEBUG
        qDebug("property name: %s id: %d", ps->getName().latin1(), ps->getID());
#endif

        if (!getPropertyData(ps->getID()).length())
        {
#ifdef DEBUG
            qDebug("skipped");
#endif

            i++;
            continue;
        }

        if (ps->isOneLine())
        {
            *formatedData += "<b>" + ps->getName() + ":</b> " + getPropertyData(ps->getID()) + "<br>";
        }
        else
        {
            *formatedData += "<b>" + ps->getName() + ":</b><br>" + QString(getPropertyData(ps->getID())).replace(QRegExp("\n"), "<br>") + "<br>";
        }

        i++;
    }

    return *formatedData;
}

// }}}

// {{{ class Notes

Notes::Notes(QWidget* parent, const char* name, WFlags fl)
        : QWidget(parent, name, fl)
{
    if (!name)
        setName("NotesBase");
    //    resize(364, 302);
    setCaption(tr("Form1"));
    setIconText(tr(""));

    notesDestroyed = true;
    clipboardNote = 0;
	viewType = setViewType = HALF_VIEW;
}

Notes::~Notes()
{
    if (clipboardNote)
        delete clipboardNote;

    /*
    delete noteFormatedText;
    delete noteText;
    delete noteSketch;
    delete notesTree;

    if (taskTree)
        delete taskTree;
    if (eventTree)
        delete eventTree;
    if (searchTree)
        delete searchTree;
    */
}

void Notes::clearTree()
{
    notesTree->clear();
}

// {{{ Expand Tree

void Notes::expandTree()
{
    QListView *currTree = searchTree ? searchTree : notesTree;
    NotesViewItem *currItem = static_cast<NotesViewItem *>(currTree->currentItem());

    //  currTree->setUpdatesEnabled(false);

    QListViewItemIterator *it = new QListViewItemIterator(currTree);
    for (; it->current(); ++(*it))
    {
        it->current()->setOpen(true);
    }

    currTree->setCurrentItem(currItem);
    currTree->ensureItemVisible(currTree->currentItem());

    //  currTree->setUpdatesEnabled(true);
    //  currTree->update();

    notesTree->setFocus();
    delete it;
}

// }}}

// {{{ Collapse Tree

void Notes::collapseTree()
{
    QListView *currTree = searchTree ? searchTree : notesTree;

    currTree->hide();
    // It's not the "root" item
    if (currTree->currentItem()->parent())
    {
        currTree->setCurrentItem(currTree->firstChild());
    }

    QListViewItemIterator *it = new QListViewItemIterator(currTree);
    for (; it->current(); ++(*it))
    {
        it->current()->setOpen(false);
    }

    currTree->ensureItemVisible(currTree->currentItem());
    currTree->show();
    notesTree->setFocus();

    delete it;
}

// }}}

// {{{ Task List

void Notes::taskListClose()
{
    if (taskTree)
    {
        if (taskTree->currentItem())
            setCurrentItem(static_cast<NotesViewItem *>(taskTree->currentItem())->getID());

        delete taskTree;
        taskTree = 0;

        if (viewType != HIDE_TREE)
            notesTree->show();

        noteChanged(notesTree->currentItem());

        emit taskListClosed();

        refreshItems();

        return;
    }
}

void Notes::taskListWholeTree()
{
    taskList();
}

void Notes::taskListCurrent()
{
    if (notesTree->currentItem()->firstChild())
        taskList(static_cast<NotesViewItem *>(notesTree->currentItem()->firstChild()));
}

void Notes::taskList(NotesViewItem *startItem)
{
#ifdef DESKTOP
    taskTree = new QListView(splitter);
#else
	taskTree = new QListView(this);
#endif
    taskTree->addColumn("bla");
    taskTree->header()->hide();
    taskTree->setSorting(-1);
    if (!showScrollBars)
    {
        taskTree->setVScrollBarMode(QScrollView::AlwaysOff);
        taskTree->setHScrollBarMode(QScrollView::AlwaysOff);
    }

    taskTree->setGeometry(notesTree->geometry());
    connect(taskTree, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(noteChanged(QListViewItem *)));

    taskListRecur(startItem);

    if (!taskTree->firstChild())
    {
        QMessageBox::warning(this, tr("No tasks"), tr("Can't show task list.\nNo tasks found."), 0, 0);

        delete taskTree;
        taskTree = 0;
        return;
    }

    sort(static_cast<NotesViewItem *>(taskTree->firstChild()), taskListSC, true);

    taskTree->setCurrentItem(taskTree->firstChild());
    taskTree->setFocus();
	noteChanged(taskTree->firstChild());
    emit taskListShown();

    notesTree->hide();
    taskTree->show();
#ifdef DESKTOP
	splitter->moveToFirst(taskTree);
#endif
}

void Notes::taskListRecur(NotesViewItem *startItem)
{
    /*
    QListViewItemIterator *it = new QListViewItemIterator(notesTree);
    for (; it->current(); ++(*it))
    {
    if (static_cast<NotesViewItem *>(it->current())->isTask())
    {
     NotesViewItem *item = static_cast<NotesViewItem *>(new NotesViewItem(taskTree));
     *item = *(static_cast<NotesViewItem *>(it->current()));
    }
    }
    */

    if (!startItem)
        startItem = static_cast<NotesViewItem *>(notesTree->firstChild());

    for (NotesViewItem *curr = startItem->firstSibling(); curr; curr = curr->nextSibling())
    {
        if (curr->isTask())
        {
            NotesViewItem *item = static_cast<NotesViewItem *>(new NotesViewItem(taskTree));
            *item = *curr;

            // Let's make it last
            NotesViewItem *last = item->lastSibling();
            if (last != item && last)
                item->moveItem(last);
        }

        if (curr->firstChild())
            taskListRecur(static_cast<NotesViewItem *>(curr->firstChild()));
    }
}

// }}}

// {{{ Event List

void Notes::eventListClose()
{
    if (eventTree)
    {
        if (eventTree->currentItem())
            setCurrentItem(static_cast<NotesViewItem *>(eventTree->currentItem())->getID());

        delete eventTree;
        eventTree = 0;

        if (viewType != HIDE_TREE)
            notesTree->show();

        noteChanged(notesTree->currentItem());

        emit eventListClosed();

        refreshItems();

        return;
    }
}

void Notes::eventListWholeTree()
{
    eventList();
}

void Notes::eventListCurrent()
{
    if (notesTree->currentItem()->firstChild())
        eventList(static_cast<NotesViewItem *>(notesTree->currentItem()->firstChild()));
}

void Notes::eventList(NotesViewItem *startItem)
{
#ifdef DESKTOP
    eventTree = new QListView(splitter);
#else
	eventTree = new QListView(this);
#endif

    eventTree->addColumn("bla");
    eventTree->header()->hide();
    eventTree->setSorting(-1);
    if (!showScrollBars)
    {
        eventTree->setVScrollBarMode(QScrollView::AlwaysOff);
        eventTree->setHScrollBarMode(QScrollView::AlwaysOff);
    }

    eventTree->setGeometry(notesTree->geometry());
    connect(eventTree, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(noteChanged(QListViewItem *)));

    eventListRecur(startItem);

    if (!eventTree->firstChild())
    {
        QMessageBox::warning(this, tr("No events"), tr("Can't show event list.\nNo events found."), 0, 0);

        delete eventTree;
        eventTree = 0;
        return;
    }

    sort(static_cast<NotesViewItem *>(eventTree->firstChild()), eventListSC, true);

    eventTree->setCurrentItem(eventTree->firstChild());
    eventTree->setFocus();
	noteChanged(eventTree->firstChild());
    emit eventListShown();

    notesTree->hide();
    eventTree->show();
#ifdef DESKTOP
	splitter->moveToFirst(eventTree);
#endif
}

void Notes::eventListRecur(NotesViewItem *startItem)
{
    /*
    QListViewItemIterator *it = new QListViewItemIterator(notesTree);
    for (; it->current(); ++(*it))
    {
    if (static_cast<NotesViewItem *>(it->current())->isEvent())
    {
     NotesViewItem *item = static_cast<NotesViewItem *>(new NotesViewItem(eventTree));
     *item = *(static_cast<NotesViewItem *>(it->current()));
    }
    }
    */

    if (!startItem)
        startItem = static_cast<NotesViewItem *>(notesTree->firstChild());

    for (NotesViewItem *curr = startItem; curr; curr = curr->nextSibling())
    {
        if (curr->isEvent())
        {
            NotesViewItem *item = static_cast<NotesViewItem *>(new NotesViewItem(eventTree));
            *item = *curr;

            // Let's make it last
            NotesViewItem *last = item->lastSibling();
            if (last != item && last)
                item->moveItem(last);
        }

        if (curr->firstChild())
            eventListRecur(static_cast<NotesViewItem *>(curr->firstChild()));
    }
}

// }}}

void Notes::showReminder(bool showDialogIfNone)
{
    QDateTime currDateTime = QDateTime::currentDateTime();

#ifndef DESKTOP
    reminderTree = new QListView(this);
#else
	reminderTree = new QListView(splitter);
#endif
    reminderTree->addColumn("bla");
    reminderTree->header()->hide();
    reminderTree->setSorting(-1);
    if (!showScrollBars)
    {
        reminderTree->setVScrollBarMode(QScrollView::AlwaysOff);
        reminderTree->setHScrollBarMode(QScrollView::AlwaysOff);
    }

    reminderTree->setGeometry(notesTree->geometry());
    connect(reminderTree, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(noteChanged(QListViewItem *)));

    QListViewItemIterator *it = new QListViewItemIterator(notesTree);
    for (; it->current(); ++(*it))
    {
        NotesViewItem *currItem = static_cast<NotesViewItem *>(it->current());
        if (currItem->hasReminder() && currItem->getReminder() <= currDateTime)
        {
            NotesViewItem *item = static_cast<NotesViewItem *>(new NotesViewItem(reminderTree));
            *item = *(static_cast<NotesViewItem *>(it->current()));
        }
    }
    
    if (!reminderTree->firstChild())
    {
        if (showDialogIfNone)
            QMessageBox::warning(this, tr("Reminder"), tr("Reminder list is empty"), 0, 0);

        delete reminderTree;
        reminderTree = 0;
        return;
    }
    
    sort(static_cast<NotesViewItem *>(reminderTree->firstChild()), reminderSC, true);
    
    reminderTree->setCurrentItem(reminderTree->firstChild());
    reminderTree->setFocus();
	noteChanged(reminderTree->firstChild());
    emit reminderShown();

    notesTree->hide();
    reminderTree->show();
#ifdef DESKTOP
	splitter->moveToFirst(reminderTree);
#endif
}

void Notes::reminderClose()
{
    if (reminderTree)
    {
        if (reminderTree->currentItem())
            setCurrentItem(static_cast<NotesViewItem *>(reminderTree->currentItem())->getID());

        delete reminderTree;
        reminderTree = 0;

        if (viewType != HIDE_TREE)
            notesTree->show();

        noteChanged(notesTree->currentItem());

        emit reminderClosed();

        refreshItems();

        return;
    }
}


void Notes::destroyNotes()
{
    notesDestroyed = true;
	qDebug("Destroy");
#ifdef DESKTOP
	delete splitter;
	delete vBoxLayout;
#else
	delete notesTree;
    delete noteFormatedText;
    delete noteText;
    delete noteSketch;
#endif	
    

    notesTree = 0;
    noteFormatedText = 0;
    noteText = 0;
    noteSketch = 0;
#ifdef DESKTOP
	splitter = 0;
	vBoxLayout = 0;
#endif

    if (taskTree)
        delete taskTree;
    if (eventTree)
        delete eventTree;
    if (searchTree)
        delete searchTree;
    if (reminderTree)
        delete reminderTree;

    searchTree = 0;
    taskTree = 0;
    eventTree = 0;
    reminderTree = 0;
}

void Notes::createNotes()
{
    notesDestroyed = false;
	QWidget *p; // temporary parent holder
	
	viewType = setViewType = HALF_VIEW;
	
#ifdef DESKTOP
	vBoxLayout = new QVBoxLayout(this);
	
	splitter = new QSplitter(this);
	vBoxLayout->addWidget(splitter);
	
	p = splitter;
#else
	p = this;
#endif

    notesTree = new QListView(p, "notesTree");
    notesTree->addColumn(tr("Column 1"));
    notesTree->header()->hide();
    notesTree->setSorting(-1);
    notesTree->setFocus();

    noteFormatedText = new QTextView(p, "noteData");
    noteText = new QMultiLineEdit(p);
    noteText->setReadOnly(true);
    noteSketch = new QSketch(p);
    noteSketch->setReadOnly();
	noteSketch->setMoveMode(true);

    connect(notesTree, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(noteChanged(QListViewItem *)));
    connect(notesTree, SIGNAL(returnPressed(QListViewItem *)), this, SLOT(editNote(QListViewItem *)));
    connect(notesTree, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(editNote(QListViewItem *)));

    searchTree = 0;
    taskTree = 0;
    eventTree = 0;
    reminderTree = 0;

    currentParent = 0;

    setWordWrap(true);
    setShowScrollBars(true);

    halfView();
}


void Notes::setWordWrap(bool wrap)
{
    wordWrap = wrap;
    if (wordWrap)
	{
        noteText->setWordWrap(QMultiLineEdit::WidgetWidth);
		noteText->setWrapPolicy(QMultiLineEdit::Anywhere);
	}
    else
        noteText->setWordWrap(QMultiLineEdit::NoWrap);
}

void Notes::setRootNodeDecoration(bool rootDecoration)
{
    notesTree->setRootIsDecorated(rootDecoration);
}

void Notes::setShowScrollBars(bool scrollBars)
{
    showScrollBars = scrollBars;
    QListView *currentTree = getCurrentTree();
    QScrollView::ScrollBarMode sv = scrollBars ? QScrollView::Auto : QScrollView::AlwaysOff;
    
    currentTree->setVScrollBarMode(sv);
    currentTree->setHScrollBarMode(sv);
    currentTree->ensureItemVisible(currentTree->currentItem());
    currentTree->triggerUpdate();

    if (currentTree != notesTree)
    {
        notesTree->setVScrollBarMode(sv);
        notesTree->setHScrollBarMode(sv);
        notesTree->ensureItemVisible(notesTree->currentItem());
        notesTree->triggerUpdate();
    }
}

void Notes::setVerticalLayout(bool verticalL)
{
	verticalLayout = verticalL;
	setViewType = HALF_VIEW;
	repaint();
}

void Notes::sort(NotesViewItem *thisLevel, SortControl sc, bool goDown)
{
    bool swapped;

    if (!thisLevel)
        return;

    // Sort Current Level
    do
    {
        swapped = false;
        for (NotesViewItem *curr = thisLevel->firstSibling(); curr; curr = curr->nextSibling())
        {
            NotesViewItem *nextSibling = curr->nextSibling();

            if (nextSibling)
            {
                int comp = curr->compare(nextSibling, sc);

                // goes down
                if (comp > 0)
                {
                    curr->moveItem(nextSibling);
                    swapped = true;
                }

            }

        }
    }
    while (swapped);

    // Sort down level
    if (goDown)
    {
        for (NotesViewItem *curr = thisLevel->firstSibling(); curr; curr = curr->nextSibling())
        {
            sort(static_cast<NotesViewItem *>(curr->firstChild()), sc, goDown);
        }
    }

    /*
    bool swapped;
    int nonTasksValue;

    if (!thisLevel)
    return;

    if ((nonTasks == SortTasks::NON_TASKS_FIRST && order == SortTasks::ORDER_ASCEND) ||
      (nonTasks == SortTasks::NON_TASKS_LAST && order == SortTasks::ORDER_DESCEND))
    nonTasksValue = -1;
    else // if ((nonTasksFirst && !ascend) || (!nonTasksFirst && ascend))
    nonTasksValue = 101;

    // Sort Current Level
    do
    {
    swapped = false;
    for (NotesViewItem *curr = thisLevel->firstSibling(); curr; curr = static_cast<NotesViewItem *>(curr->nextSibling()))
    {
      NotesViewItem *nextSibling = static_cast<NotesViewItem *>(curr->nextSibling());

      if (nextSibling)
      {
    int currPer, nextPer;

    if (curr->isTask())
    {
    currPer = curr->getPercentageInt();
    }
    else
    currPer = nonTasksValue;

    if (nextSibling->isTask())
    {
    nextPer = nextSibling->getPercentageInt();
    }
    else
    nextPer = nonTasksValue;

    if ((order == SortTasks::ORDER_ASCEND && currPer > nextPer) ||
     (order == SortTasks::ORDER_DESCEND && currPer < nextPer))
    {
    curr->moveItem(nextSibling);
    swapped = true;
    }
      }

    }
    } while (swapped);

    // Sort down level
    if (goDown)
    {
    for (NotesViewItem *curr = thisLevel->firstSibling(); curr; curr = static_cast<NotesViewItem *>(curr->nextSibling()))
    {
      sortTasks(static_cast<NotesViewItem *>(curr->firstChild()), order, nonTasks, goDown);
    }
    }
    */
}

void Notes::sort()
{
    // static SortControl *tasksSortControl = 0, *eventsSortControl = 0;

    Sort s(this, 0, true);
    NotesViewItem *startItem;
    bool recur;

    if (taskTree)
    {
        s.Tabs->setTabEnabled(s.EventsTab, false);
        s.Tabs->setTabEnabled(s.OthersTab, false);
        s.Tabs->setTabEnabled(s.SettingTab, false);

        s.setSortControl(taskListSC);
    }
    else if (eventTree)
    {
        s.Tabs->setTabEnabled(s.TasksTab, false);
        s.Tabs->setTabEnabled(s.OthersTab, false);
        s.Tabs->setTabEnabled(s.SettingTab, false);

        s.setSortControl(eventListSC);
    }
    else if (reminderTree)
    {
        s.setSortControl(reminderSC);
    }
    else
        s.setSortControl(mainSC);

#ifndef DESKTOP
    s.showMaximized();
#endif
    if (!s.exec())
        return;

    if (taskTree)
        taskListSC = s.getSortControl();
    else if (eventTree)
        eventListSC = s.getSortControl();
    else if (reminderTree)
        reminderSC = s.getSortControl();
    else
        mainSC = s.getSortControl();

    QListView *currTree = getCurrentTree();

    // current level
    if (s.SortType->currentItem() == 0)
    {
        recur = false;
        startItem = static_cast<NotesViewItem *>(currTree->currentItem());
    }
    // current level and down
    else if (s.SortType->currentItem() == 1)
    {
        recur = true;
        startItem = static_cast<NotesViewItem *>(currTree->currentItem());
    }
    // whole tree
    else
    {
        recur = true;
        startItem = static_cast<NotesViewItem *>(currTree->firstChild());
    }

    sort(startItem, s.getSortControl(), recur);
}

// {{{ View methods

void Notes::paintEvent(QPaintEvent *pe)
{
    if (notesDestroyed)
        return;
    
    if (!pe->erased())
        return;

#ifdef DESKTOP
    splitter->show();
#endif

#ifndef DESKTOP
    uint w = width(), h = height(), hh = h >> 1, ww = w >> 1;
#endif

	if (setViewType == HALF_VIEW)
    {
		if (verticalLayout)
		{
#ifdef DESKTOP
			splitter->setOrientation(QSplitter::Vertical);
#else
			noteFormatedText->setGeometry(QRect(0, hh, w, hh));
			noteText->setGeometry(QRect(0, hh, w, hh));
			noteSketch->setGeometry(QRect(0, hh, w, hh));
		
			notesTree->setGeometry(QRect(0, 0, w, hh));
#endif
		}
		else
		{
#ifdef DESKTOP
			splitter->setOrientation(QSplitter::Horizontal);
#else
			noteFormatedText->setGeometry(QRect(ww, 0, ww, h));
			noteText->setGeometry(QRect(ww, 0, ww, h));
			noteSketch->setGeometry(QRect(ww, 0, ww, h));
		
			notesTree->setGeometry(QRect(0, 0, ww, h));
#endif
		}

        QListView *currTree = getCurrentTree();
        if (currTree != notesTree)
        {
            currTree->setGeometry(notesTree->geometry());
        }
        currTree->show();
        
        if (!notesTree->childCount())
        {
            noteText->show();
        }

        noteChanged(currTree->currentItem());
    }
    else if (setViewType == HIDE_NOTE)
    {
        noteFormatedText->hide();
        noteText->hide();
        noteSketch->hide();

#ifndef DESKTOP
        noteFormatedText->setGeometry(QRect(0, 0, 0, 0));
        noteText->setGeometry(QRect(0, 0, 0, 0));
        noteSketch->setGeometry(QRect(0, 0, 0, 0));

        notesTree->setGeometry(QRect(0, 0, w, h));
#endif

        QListView *currTree = getCurrentTree();
        if (currTree != notesTree)
        {
            currTree->setGeometry(notesTree->geometry());
        }

        currTree->show();
    }
    else if (setViewType == HIDE_TREE)
    {
        QListView *currTree = getCurrentTree();

        currTree->hide();

#ifndef DESKTOP
        notesTree->setGeometry(QRect(0, 0, w, h));
#endif
        if (currTree != notesTree)
        {
            currTree->setGeometry(notesTree->geometry());
        }

        if (!notesTree->childCount())
        {
            noteText->show();
        }

#ifndef DESKTOP
        noteFormatedText->setGeometry(QRect(0, 0, w, h));
        noteText->setGeometry(QRect(0, 0, w, h));
        noteSketch->setGeometry(QRect(0, 0, w, h));
#endif
        noteChanged(currTree->currentItem());
    }

    if (setViewType != NONE)
    {
#ifdef DESKTOP
		noteChanged(getCurrentTree()->currentItem());
#endif
        viewType = setViewType;
        setViewType = NONE;
    }
}

void Notes::resizeEvent(QResizeEvent *re)
{
    if (viewType != NONE)
        setViewType = viewType;
    else
        setViewType = HALF_VIEW;

    repaint();
}

void Notes::halfView()
{
    setViewType = HALF_VIEW;
    repaint(true);
#ifdef DESKTOP
	if (splitterSize.count()) {
		splitter->setSizes(splitterSize);
		splitterSize.clear();
	}
#endif
}

void Notes::fullTreeView()
{
#ifdef DESKTOP
	splitterSize = splitter->sizes();
#endif
    setViewType = HIDE_NOTE;
    repaint(true);
}

void Notes::fullNoteView()
{
    setViewType = HIDE_TREE;
    repaint(true);
#ifdef DESKTOP
	if (splitterSize.count()) {
		splitter->setSizes(splitterSize);
		splitterSize.clear();
	}
#endif //DESKTOP
}

// }}}

bool Notes::setPicture()
{
    QListView *currTree = getCurrentTree();
    NotesViewItem *currentItem = static_cast<NotesViewItem *>(currTree->currentItem());

    ChoosePic cp(this, 0, true);
    if (currentItem->hasPic())
	cp.setCurrentPic(currentItem->getPicFile());
#ifndef DESKTOP
    cp.showMaximized();
#else
    cp.adjustSize();
#endif
    if (!cp.exec())
        return false;

    currentItem->setMyPixmap(cp.picFile());

    return true;
}

bool Notes::setTask()
{
    QListView *currTree = getCurrentTree();

    SetTask st(this, 0, true);

    NotesViewItem *currentItem = static_cast<NotesViewItem *>(currTree->currentItem());

    if (!currentItem)
        return false;

    if (currentItem->isTask())
    {
        st.setPercentage(currentItem->getPercentage());
        st.setPriority(currentItem->getPriority());
    }

    if (!st.exec())
        return false;

    currentItem->setPercentage(st.getPercentage());
    currentItem->setPriority(st.getPriority());

    return true;
}

bool Notes::setEvent()
{
    QListView *currTree = getCurrentTree();

    SetEvent se(this, 0, true);

    NotesViewItem *currentItem = static_cast<NotesViewItem *>(currTree->currentItem());

    if (!currentItem)
        return false;

    if (currentItem->isEvent())
    {
        se.setDateTime(currentItem->getEvent().getDateTime());

        // TODO: grr, rewrite to be more in object flavor
        if(!currentItem->getEvent().hasTime())
        {
            se.IgnoreTime->setChecked(true);
            se.ignoreTimeClicked();
        }
    }

    if (!se.exec())
        return false;

    currentItem->setEvent(se.getDateTime());

    return true;
}

bool Notes::unsetTaskEvent()
{
    QListView *currTree = getCurrentTree();

    NotesViewItem *currentItem = static_cast<NotesViewItem *>(currTree->currentItem());

    if (!currentItem)
        return false;

    currentItem->unsetTask();
    currentItem->unsetEvent();

    return true;
}

bool Notes::setReminder()
{
    QListView *currTree = getCurrentTree();
    
    NotesViewItem *currentItem = static_cast<NotesViewItem *>(currTree->currentItem());

    if (!currentItem)
        return false;
    
    SetReminder sr(this, 0, true);
    
    if (currentItem->hasReminder())
    {
        sr.setDateTime(currentItem->getReminder().getDateTime());
		sr.setAlarm(currentItem->getReminder().hasAlarm());

        // TODO: grr, rewrite to be more in object flavor
        if(!currentItem->getReminder().hasTime())
        {
            sr.IgnoreTime->setChecked(true);
            sr.ignoreTimeClicked();
        }
    }

	// alarm status
	if (currentItem->getReminder().hasAlarm())
		sr.setAlarm(true);
	else
		sr.setAlarm(false);
    
    if (!sr.exec())
        return false;
        
//    currentItem->setReminder(Reminder(sr.getDateTime(), sr.Alarm->isChecked()));
    currentItem->setReminder(Reminder(sr.getDateTime(), false));
	// set alarm
    /*
	if (sr.Alarm->isChecked())
	{
		AlarmServer::addAlarm(sr.getDateTime(), QCString("QPE/Application/iqnotes-alarm"), QCString(currentItem->text(0)));
	}
    */
    
    return true;
}

bool Notes::unsetReminder()
{
    QListView *currTree = getCurrentTree();

    NotesViewItem *currentItem = static_cast<NotesViewItem *>(currTree->currentItem());

    if (!currentItem)
        return false;
    
    currentItem->unsetReminder();

    return true;
}


bool Notes::isEntryUsed(const QString &entryName)
{
    QListViewItemIterator it(notesTree);

    for (; it.current(); ++it)
    {
        if (static_cast<NotesViewItem *>(it.current())->getEntryName() == entryName)
            return true;
    }

    return false;
}

bool Notes::searchRecur(NotesViewItem *firstChild, NotesViewItem *parentInSearchTree)
{
    bool foundGlobal = false;

    for (NotesViewItem *c = firstChild; c; c = c->nextSibling())
    {
        bool found = false, retFound = false;

        NotesViewItem *nvi1 = new NotesViewItem(parentInSearchTree), *last;

        // Let's make it last
        last = nvi1->lastSibling();
        if (last != nvi1 && last)
            nvi1->moveItem(last);

        *nvi1 = *c;
        //      nvi1->setText(0, c->text(0));
        //      nvi1->setData(c->getData());
        //      nvi1->setEntryName(c->getEntryName());
        //      nvi1->setPercentage(c->getPercentagePtr());

        if (c->searchFor(searchPattern))
        {
            nvi1->setMatched(true);
            foundGlobal = true;
            found = true;
        }
        else
            nvi1->setMatched(false);

        if (!(retFound = searchRecur(static_cast<NotesViewItem *>(c->firstChild()), nvi1)) && !found)
            delete nvi1;

        if (retFound)
            foundGlobal = true;
    }

    return foundGlobal;
}

void Notes::search()
{
    // Close search tree
    if (searchTree)
    {
        if (searchTree->currentItem())
        {
            setCurrentItem(static_cast<NotesViewItem *>(searchTree->currentItem())->getID());
        }

        closeSearchTree();

        NoteData::lastID = noteDataLastID;

        return;
    }

    SearchBase sb(this, 0, true);

    if (!notesTree->currentItem()->childCount())
        sb.FromCurrentNote->setEnabled(false);

    if (!sb.exec())
    {
        return;
    }

    noteDataLastID = NoteData::lastID;

    searchPattern = sb.SearchFor->text();

#ifdef DESKTOP
	searchTree = new QListView(splitter);
#else
    searchTree = new QListView(this);
#endif
    searchTree->addColumn("bla");
    searchTree->header()->hide();
    searchTree->setSorting(-1);
    if (!showScrollBars)
    {
        searchTree->setVScrollBarMode(QScrollView::AlwaysOff);
        searchTree->setHScrollBarMode(QScrollView::AlwaysOff);
    }

    searchTree->setGeometry(notesTree->geometry());
    connect(searchTree, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(noteChanged(QListViewItem *)));
    notesTree->hide();
    searchTree->show();
#ifdef DESKTOP
	splitter->moveToFirst(searchTree);
#endif

    forSearchRecur = new NotesViewItem(searchTree);
    forSearchRecur->setText(0, tr("Search result"));
    forSearchRecur->setTextData(tr("Search result for : ") + searchPattern);

    if (!sb.FromCurrentNote->isChecked())
    {
        searchRecur(static_cast<NotesViewItem *>(notesTree->firstChild()), forSearchRecur);
    }
    else
    {
        searchRecur(static_cast<NotesViewItem *>(notesTree->currentItem()->firstChild()), forSearchRecur);
    }

    searchTree->setCurrentItem(searchTree->firstChild());

    expandTree();

    emit searchTreeShown();
}

void Notes::deleteEntryProperties(const QString &entryName, QValueList<int> *deletedProperties)
{
    if (!deletedProperties->count())
        return;

    QListViewItemIterator it(notesTree);

    for (; it.current(); ++it)
    {
        NotesViewItem *nvi = static_cast<NotesViewItem *>(it.current());
        if (nvi->getEntryName() == entryName)
        {
            nvi->deleteEntryProperties(deletedProperties);
        }
    }
}

void Notes::closeSearchTree()
{
    delete searchTree;
    searchTree = 0;

    if (viewType != HIDE_TREE)
        notesTree->show();

    noteChanged(notesTree->currentItem());

    emit searchTreeClosed();

    refreshItems();
}

bool Notes::writeNote(NotesViewItem *nvi)
{
    NoteType nt(this, 0, true);
    Entry *entry;
    Entries *entriesList = IQApp->entries();

    // Hardcore note's type
    nt.addNoteType("Text");
    nt.addNoteType("Sketch");

    if (entriesList->count())
    {
        for (entry = entriesList->first(); entry; entry = entriesList->next())
        {
	    if (entry->getDefaultPic() != "")
		nt.addNoteType(Resource::loadPixmap("iqnotes/items/" + entry->getDefaultPic()), entry->getName());
	    else
		nt.addNoteType(entry->getName());
        }
    }

#ifndef DESKTOP
    nt.showMaximized();
#else
    nt.adjustSize();
#endif
    if (!nt.exec())
        return false;

    // Text note
    if (nt.getTypeInt() == 0)
    {
        WriteNoteBase wn(this, 0, true);
#ifndef DESKTOP
        wn.showMaximized();
#endif
        if (!wn.exec())
            return false;

        nvi->setTextData(wn.Note->text());
    }
    // Sketch type
    else if (nt.getTypeInt() == 1)
    {
        WriteSketchNote wsn(this, 0, true);

#ifndef DESKTOP
        wsn.showMaximized();
#endif
        if (!wsn.exec())
            return false;

        nvi->setStrokes(wsn.getStrokes());
    }
    // Defined entry
    else
    {
        WriteDefinedEntry wde(this, 0, true);
#ifndef DESKTOP
        wde.showMaximized();
#endif

        entry = entriesList->at(nt.getTypeInt() - 2);

        // Fill dialog with properties
        PropertyStruct *ps;
        for (ps = entry->first(); ps; ps = entry->next())
        {
            if (ps->getType() == PropertyStruct::ONE_LINE)
                wde.addOneLine(ps->getName(), PropertyData("", ps->getID()));
            else
                wde.addMultiLine(ps->getName(), PropertyData("", ps->getID()));
        }

        wde.setEntryName(entry->getName());
        if (!wde.exec())
            return false;

        // Setup Note data
        nvi->setEntryName(entry->getName());
        nvi->setData(wde.getDataList());
	nvi->setMyPixmap(entry->getDefaultPic());
    }

    // Make new item current
    notesTree->ensureItemVisible(nvi);
    notesTree->setCurrentItem(nvi);

    return true;
}

bool Notes::renameNote()
{
    QListView *currTree = getCurrentTree();

    NotesViewItem *nvi = static_cast<NotesViewItem *>(currTree->currentItem());

    RenameNoteBase rename(this, 0, true);

    rename.NoteName->setText(nvi->text(0));
    rename.adjustSize();
    if(!rename.exec())
        return false;

    nvi->setText(0, rename.NoteName->text());

    return true;
}

bool Notes::editNote()
{
    return editNote(getCurrentTree()->currentItem());
}

bool Notes::editNote(QListViewItem *lvi)
{
    if (! lvi)
        return false;

    NotesViewItem *nvi = static_cast<NotesViewItem *>(lvi);

    // Text note
    if (nvi->isText())
    {
        WriteNoteBase wn(this, 0, true);
        wn.Note->setWordWrap(wordWrap ? QMultiLineEdit::WidgetWidth : QMultiLineEdit::NoWrap);
        wn.Note->setText(nvi->getTextData());
#ifndef DESKTOP
        wn.showMaximized();
#endif
        if (!wn.exec())
            return false;

        nvi->setTextData(wn.Note->text());
    }
    // Sketch
    else if (nvi->isSketch())
    {
        Strokes *strokes = new Strokes(), *strokes1 = nvi->getStrokes();
        WriteSketchNote wsn(this, 0, true);

        // TODO: rewrite to copy constructor
        for (Stroke *stroke = strokes1->first(); (stroke = strokes1->current()); strokes1->next())
        {
            strokes->addStroke(stroke->serialize());
        }

        wsn.setStrokes(strokes);

#ifndef DESKTOP
        wsn.showMaximized();
#endif
        if (!wsn.exec())
        {
            delete strokes;
            return false;
        }

        nvi->setStrokes(strokes);
    }
    // Defined entry
    else
    {
        WriteDefinedEntry wde(this, 0, true);
        Entries *entriesList = IQApp->entries();
        Entry *entry;

        // Find entry
        for (entry = entriesList->first(); entry; entry = entriesList->next())
        {
            if (entry->getName() == nvi->getEntryName())
                break;
        }

        // Fill dialog
        PropertyStruct *ps;
        int i = 0;
        for (ps = entry->first(); ps; ps = entry->next())
        {
            if (ps->getType() == PropertyStruct::ONE_LINE)
                wde.addOneLine(ps->getName(), *(nvi->getNoteData()->getPropertyData(ps->getID())));
            else
                wde.addMultiLine(ps->getName(), *(nvi->getNoteData()->getPropertyData(ps->getID())));

            i++;
        }

        wde.setEntryName(entry->getName());

#ifndef DESKTOP
        wde.showMaximized();
#endif
        if (!wde.exec())
            return false;

        // Setup Note data
        nvi->setData(wde.getDataList());
    }

    noteChanged(nvi);
    emit noteModified(true);
    return true;
}

void Notes::noteChanged(QListViewItem *lvi)
{
#ifdef DEBUG
    qDebug("void Notes::noteChanged(QListViewItem *lvi)");
#endif

    if (!notesTree->childCount())
    {
#ifdef DEBUG
        qDebug("no childs");
#endif

        noteSketch->clearStrokes();
        noteFormatedText->setText("");
        noteText->setText("");
        noteSketch->hide();
        noteFormatedText->hide();
        return;
    }

    if (!lvi)
    {
#ifdef DEBUG
		qDebug("null current item!");
#endif
		return;
    }

    NotesViewItem *nvi = static_cast<NotesViewItem *>(lvi);

#ifdef DESKTOP
    if (viewType == HIDE_NOTE)
        return;

    QRect g;
    if (noteFormatedText->isShown())
        g = noteFormatedText->geometry();
    else if (noteText->isShown())
        g = noteText->geometry();
    else if (noteSketch->isShown())
        g = noteSketch->geometry();
    QValueList<int> sizes = splitter->sizes();
    for (uint i = 1; i < sizes.count(); i++)
        if (splitter->orientation() != Qt::Vertical)
            sizes[i] = g.width();
        else
            sizes[i] = g.height();
#endif

    // Note is sketch
    if (nvi->isSketch())
    {
        noteSketch->setStrokes(nvi->getStrokes());
        noteSketch->repaint();
        noteSketch->show();
        noteFormatedText->hide();
        noteText->hide();
    }
    // normal note
    else
    {
        if (!nvi->getEntryName().length())
        {
            noteText->setText(nvi->getFormatedData());
            noteFormatedText->hide();
            noteText->show();
        }
        else
        {
            noteFormatedText->setTextFormat(Qt::RichText);
            noteFormatedText->setText(nvi->getFormatedData());
            noteText->hide();
            noteFormatedText->show();
        }

        noteSketch->hide();
    }

    getCurrentTree()->setFocus();
#ifdef DESKTOP
    splitter->setSizes(sizes);
#endif
}

QListView *Notes::getCurrentTree()
{
    if (searchTree)
        return searchTree;
    else if (taskTree)
        return taskTree;
    else if (eventTree)
        return eventTree;
    else if (reminderTree)
        return reminderTree;

    return notesTree;
}

void Notes::saveTreeRecur(NotesViewItem *nvi, QTextStream *saveData)
{
    for (NotesViewItem *nvi1 = nvi; nvi1; nvi1 = nvi1->nextSibling())
    {
        QString s;
        *saveData << "<note name=\"" << (App::quoteXML(nvi1->text(0))) << "\" "
        << "type=\"";

        if (nvi1->isSketch())
        {
            *saveData << "Sketch\" ";
        }
        else if (nvi1->isText())
        {
            *saveData << "Text\" ";
        }
        else
        {
            *saveData << (App::quoteXML(nvi1->getEntryName())) << "\" ";
        }

        if (nvi1->isTask())
        {
            *saveData << "percentage=\"" << App::quoteXML(nvi1->getPercentage()) << "\" "
            << "priority=\"" << nvi1->getPriority() << "\" ";
        }
        else if (nvi1->isEvent())
        {
            QDateTime dt = nvi1->getEvent().getDateTime();

            *saveData << "day=\"" << dt.date().day() << "\" "
            << "month=\"" << dt.date().month() << "\" "
            << "year=\"" << dt.date().year() << "\" ";

            if (nvi1->getEvent().hasTime())
            {
                *saveData << "hour=\"" << dt.time().hour() << "\" "
                << "min=\"" << dt.time().minute() << "\" ";
            }
        }
        
        if (nvi1->hasReminder())
        {
            QDateTime dt = nvi1->getReminder().getDateTime();

            *saveData << "rday=\"" << dt.date().day() << "\" "
            << "rmonth=\"" << dt.date().month() << "\" "
            << "ryear=\"" << dt.date().year() << "\" ";

            if (nvi1->getReminder().hasTime())
            {
                *saveData << "rhour=\"" << dt.time().hour() << "\" "
                << "rmin=\"" << dt.time().minute() << "\" ";
            }
        }

        if (nvi1->hasPic())
        {
            *saveData << "pic=\"" << nvi1->getPicFile() << "\" ";
        }

        if (nvi1->isText())
        {
            *saveData << "data0=\"" << (App::quoteXML(nvi1->getTextData())) << "\" ";
        }
        else if (!nvi1->isSketch())
        {
            Entries *entries = IQApp->entries();
            Entry *entry = entries->isIn(nvi1->getEntryName());

            int i = 0;
            for (PropertyStruct *ps = entry->first(); ps; ps = entry->next(), i++)
            {
                *saveData << "data" << i << "=\"" << (App::quoteXML(nvi1->getPropertyData(ps->getID()))) << "\" ";
            }

            /*
            QList<PropertyData> *data = nvi1->getNoteData()->getData();
            for (uint i = 0; i < data->count(); i++)
            {
                *saveData << "data" << i << "=\"" << (App::quoteXML(data->at(i)->getData())) << "\" ";
            }
            */
        }
        else
        {
            *saveData << nvi1->getStrokes()->serialize();
        }

        *saveData << ">\n";
        saveTreeRecur(static_cast<NotesViewItem *>(nvi1->firstChild()), saveData);
        *saveData << "</note>\n";
    }
}

void Notes::saveTree(QTextStream *saveData)
{
    *saveData << "<notes>\n";
    saveTreeRecur(static_cast<NotesViewItem *>(notesTree->firstChild()), saveData);
    *saveData << "</notes>\n";
}

void Notes::addNote(const QString &noteName, QList<QString> *data1, const QString &entryName, const QString &percentage, int prio, QDateTime dt, const QString &picFile, QDateTime rem)
{
    NotesViewItem *nvi, *last;

    if (!currentParent)
    {
        nvi = new NotesViewItem(notesTree);
    }
    else
    {
        nvi = new NotesViewItem(currentParent);
    }

    last = nvi->lastSibling();
    if (last != nvi && last)
        nvi->moveItem(last);

    nvi->setEntryName(entryName);
    nvi->setText(0, noteName);
    nvi->setData(data1);
    if (percentage.length())
    {
        nvi->setPercentage(percentage);
        nvi->setPriority(prio);
    }
    nvi->setEvent(dt);
    nvi->setReminder(rem);

    // Has pic
    if (picFile.length())
    {
        nvi->setMyPixmap(picFile);
    }

    currentParent = nvi;

    emit noEmptyNoteTree();

    if (!notesTree->currentItem())
    {
        noteChanged(notesTree->firstChild());
        //notesTree->setCurrentItem(notesTree->firstChild());
        //notesTree->setSelected(notesTree->firstChild(), true);
        notesTree->setFocus();
    }
}

void Notes::addNote(const QString &noteName, Strokes *strokes, const QString &entryName, const QString &percentage, int prio, QDateTime dt, const QString &picFile, QDateTime rem)
{
    NotesViewItem *nvi, *last;

    if (!currentParent)
    {
        nvi = new NotesViewItem(notesTree);
    }
    else
    {
        nvi = new NotesViewItem(currentParent);
    }

    last = nvi->lastSibling();
    if (last != nvi && last)
        nvi->moveItem(last);

    nvi->setEntryName(entryName);
    nvi->setText(0, noteName);
    //  nvi->setPropertyData(*data1, false);
    nvi->setStrokes(strokes);
    nvi->setPercentage(percentage);
    nvi->setPriority(prio);
    nvi->setEvent(dt);
    nvi->setReminder(rem);

    // Has pic
    if (picFile.length())
    {
        nvi->setMyPixmap(picFile);
    }


    currentParent = nvi;

    emit noEmptyNoteTree();

    if (!notesTree->currentItem())
    {
        noteChanged(notesTree->firstChild());
        //notesTree->setCurrentItem(notesTree->firstChild());
        notesTree->setFocus();
    }
}

void Notes::noNoteChild()
{
    currentParent = static_cast<NotesViewItem *>(currentParent->parent());
}

bool Notes::quickAdd()
{
	QString itemName = "Quick note";

	NotesViewItem *newItem = new NotesViewItem(notesTree);
	newItem->setText(0, itemName);

	// write text
	WriteNoteBase wn(this, 0, true);
#ifndef DESKTOP
	wn.showMaximized();
#endif
	if (!wn.exec())
	{
		delete newItem;
		return false;
	}
	newItem->setTextData(wn.Note->text());
	// Make new item current
	//notesTree->ensureItemVisible(newItem);
    //notesTree->setCurrentItem(newItem);
    noteChanged(newItem);

	emit noEmptyNoteTree();
	
	return true;
}

bool Notes::addBefore()
{
    if (!notesTree->childCount())
        return addFirst();

    QString itemName = getItemName();

    if (itemName != "")
    {
        NotesViewItem *parentItem = static_cast<NotesViewItem *>(notesTree->currentItem()->parent()), *newItem;

        // It's first item in level
        if (parentItem == notesTree->currentItem()->itemAbove())
        {
            newItem = !parentItem ? new NotesViewItem(notesTree) : new NotesViewItem(parentItem);
            newItem->setText(0, itemName);
        }
        // It's not the first
        else
        {
            newItem = !parentItem ? new NotesViewItem(notesTree, static_cast<NotesViewItem *>(notesTree->currentItem()->itemAbove())) : new NotesViewItem(notesTree, static_cast<NotesViewItem *>(notesTree->currentItem()->itemAbove()));
            newItem->setText(0, itemName);
        }

        if (!writeNote(newItem))
        {
            delete newItem;
            return false;
        }

        emit noEmptyNoteTree();
        return true;
    }
    else
    {
        return false;
    }
}

bool Notes::addAfter()
{
    if (!notesTree->childCount())
    {
        return addFirst();
    }

    QString itemName = getItemName();

    if (itemName != "")
    {
        NotesViewItem *parentItem = static_cast<NotesViewItem *>(notesTree->currentItem()->parent());
        NotesViewItem *newItem;

        if (parentItem)
        {                
            newItem = new NotesViewItem(parentItem, static_cast<NotesViewItem *>(notesTree->currentItem()));
            newItem->setText(0, itemName);
        }
        else
        {
            newItem = new NotesViewItem(notesTree, static_cast<NotesViewItem *>(notesTree->currentItem()));
            newItem->setText(0, itemName);
        }

        if (!writeNote(newItem))
        {
            delete newItem;
            return false;
        }

        emit noEmptyNoteTree();
        return true;
    }
    else
    {
        return false;
    }
}

bool Notes::addChild()
{
    NotesViewItem *newItem;

    if (!notesTree->childCount())
        return addFirst();

    QString itemName = getItemName();

    if (itemName != "")
    {
        newItem = new NotesViewItem(static_cast<NotesViewItem *>(notesTree->currentItem()));
        newItem->setText(0, itemName);
        
        if (!writeNote(newItem))
        {
            delete newItem;
            return false;
        }

        emit noEmptyNoteTree();
        return true;
    }
    else
    {
        return false;
    }
}

bool Notes::addFirst()
{
    QString itemName = getItemName();
    NotesViewItem *newItem;

    if (itemName != "")
    {
        newItem = new NotesViewItem(notesTree);
        //notesTree->insertItem(item);
        newItem->setText(0, itemName);

        if (!writeNote(newItem))
        {
            delete newItem;
            return false;
        }

        emit noEmptyNoteTree();

        noteChanged(notesTree->firstChild());
        //notesTree->setCurrentItem(notesTree->firstChild());

        return true;
    }
    else
    {
        return false;
    }
}

QString Notes::getItemName()
{
    if (!notesTree->currentItem() && notesTree->childCount())
    {
        QMessageBox::warning(this, tr("No Selected Note"), tr("Please, select the note,\nelse I can't add the new note."), 1, 0);

        return "";
    }

    QString rv;

    NoteNameBase *nm = new NoteNameBase(this, 0, true);

    if (!nm->exec())
        rv = "";
    else
    {
        rv = nm->noteName->text();
        if (rv.isNull() || rv.isEmpty())
            rv = "";
    }

    delete nm;

    return rv;
}

bool Notes::deleteNote()
{
    DeleteConfirmBase dcb(this, 0, true);

    if (!dcb.exec())
        return false;

    if (!notesTree->currentItem())
    {
        QMessageBox::warning(this, tr("No Selected Note"), tr("Please, select the note to delete."), 0, 0);

        return false;
    }

    if (clipboardNote)
        delete clipboardNote;

    NotesViewItem *parentItem, *aboveItem;

    clipboardNote = static_cast<NotesViewItem *>(notesTree->currentItem());
    aboveItem = static_cast<NotesViewItem *>(notesTree->currentItem()->itemAbove());
    if (!(parentItem = static_cast<NotesViewItem *>(clipboardNote->parent())))
        notesTree->takeItem(clipboardNote);
    else
        parentItem->takeItem(clipboardNote);

    if (!notesTree->childCount()) {
        noteChanged(0);
        emit emptyNoteTree();
    }
    else
    {
        // Make current item
        if (aboveItem)
            notesTree->setCurrentItem(aboveItem);
        else
            notesTree->setCurrentItem(notesTree->firstChild());
        notesTree->ensureItemVisible(notesTree->currentItem());

        emit noEmptyNoteTree();
    }

    return true;
}

bool Notes::copyNoteOnlyCurrent()
{
    QListView *currentTree = getCurrentTree();

    if (clipboardNote)
        delete clipboardNote;

    NoteData *nd = new NoteData(*(static_cast<NotesViewItem *>(currentTree->currentItem())->getNoteData()));
    clipboardNote = new NotesViewItem(currentTree);
    clipboardNote->setNoteData(nd, false);
    currentTree->takeItem(clipboardNote);

    return true;
}

bool Notes::copyNoteCurrentAndDownRecur(NotesViewItem *originalNote, NotesViewItem *copyNote)
{
    if (!originalNote->childCount())
        return false;

    for (NotesViewItem *currentNote = static_cast<NotesViewItem *>(originalNote->firstChild()); currentNote; currentNote = currentNote->nextSibling())
    {
        NoteData *nd = new NoteData(*(currentNote->getNoteData()));
        NotesViewItem *nvi = new NotesViewItem(copyNote);
        nvi->setNoteData(nd, false);

        // move to the end
        NotesViewItem *lastChild = nvi->lastSibling();
        if (lastChild != nvi && lastChild)
            nvi->moveItem(lastChild);

        copyNoteCurrentAndDownRecur(currentNote, nvi);
    }

    return true;
}

bool Notes::copyNoteCurrentAndDown()
{
    copyNoteOnlyCurrent();

    copyNoteCurrentAndDownRecur(static_cast<NotesViewItem *>(getCurrentTree()->currentItem()), clipboardNote);
    
    return true;
}

bool Notes::pasteNoteFirst()
{
    if (!clipboardNote)
        return false;

    if (!IQApp->foreignNodeWarning())
        return false;

    notesTree->insertItem(clipboardNote);
    clipboardNote = 0;

    refreshItems();
    emit noEmptyNoteTree();

    return true;
}

bool Notes::pasteNoteBefore()
{
    if (!clipboardNote)
        return false;

    if (!notesTree->childCount())
        return pasteNoteFirst();
            
    NotesViewItem *parentItem = static_cast<NotesViewItem *>(notesTree->currentItem()->parent());

    if (!IQApp->foreignNodeWarning())
        return false;
    
    if (!parentItem)
        notesTree->insertItem(clipboardNote);
    else
        parentItem->insertItem(clipboardNote);

    // It is not first item in level
    if (parentItem != notesTree->currentItem()->itemAbove())
        clipboardNote->moveItem(notesTree->currentItem()->itemAbove());

    clipboardNote = 0;

    refreshItems();

    return true;
}       

bool Notes::pasteNoteAfter()
{
    if (!clipboardNote)
        return false;

    if (!notesTree->childCount())
        return pasteNoteFirst();
    
    NotesViewItem *parentItem = static_cast<NotesViewItem *>(notesTree->currentItem()->parent());

    if (!IQApp->foreignNodeWarning())
        return false;
    
    if (parentItem)
        parentItem->insertItem(clipboardNote);
    else
        notesTree->insertItem(clipboardNote);
    
    clipboardNote->moveItem(notesTree->currentItem());

    refreshItems();
    
    clipboardNote = 0;

    return true;
}

bool Notes::pasteNoteChild()
{
    if (!clipboardNote)
        return false;
    
    if (!notesTree->childCount())
        return pasteNoteFirst();
    
    if (!IQApp->foreignNodeWarning())
        return false;
    
    static_cast<NotesViewItem *>(notesTree->currentItem())->insertItem(clipboardNote);
    clipboardNote = 0;

    refreshItems();

    return true;
}

void Notes::setCurrentItem(unsigned int id)
{
    QListViewItemIterator *it = new QListViewItemIterator(notesTree);
    NotesViewItem *item;

    for (; it->current(); ++(*it))
    {
        item = static_cast<NotesViewItem *>(it->current());
        if (item->getID() == id)
            break;
    }

    if (it->current())
    {
        notesTree->setCurrentItem(item);
        notesTree->ensureItemVisible(item);
    }
}

void Notes::refreshItems()
{
    QListView *currTree = getCurrentTree();
    NotesViewItem *currItem = static_cast<NotesViewItem *>(currTree->currentItem());

    //  currTree->setUpdatesEnabled(false);

    QListViewItemIterator *it = new QListViewItemIterator(currTree);
    for (; it->current(); ++(*it))
    {
        NotesViewItem *item = static_cast<NotesViewItem *>(it->current());
        item->refreshItem();
    }

    currTree->setCurrentItem(currItem);
    currTree->ensureItemVisible(currTree->currentItem());

    //  currTree->setUpdatesEnabled(true);
    //  currTree->update();

    notesTree->setFocus();
    delete it;
}

// }}}
