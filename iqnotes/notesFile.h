/*  IQNotes - Smarty notes
    Copyright (C) 2001 Peter Vrabel <kybu@kybu.sk>
    
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
#ifndef NOTESFILE_H
#define NOTESFILE_H

#include <qobject.h>
#include <qxml.h>
#include <qlist.h>
#include <qdatetime.h>

class Entry;
class NotesViewItem;
class Strokes;

class NotesFileParserError : public QXmlErrorHandler
{
public:
    NotesFileParserError()
    {
        exc = 0;
    }
    virtual bool warning ( const QXmlParseException & exception )
    {
        setExc(exception);
        return true;
    }
    virtual bool error ( const QXmlParseException & exception )
    {
        setExc(exception);
        return true;
    }
    virtual bool fatalError ( const QXmlParseException & exception )
    {
        setExc(exception);
        return true;
    }
    virtual QString errorString ()
    {
        return exc->message();
    }

private:
    void setExc(const QXmlParseException & e)
    {
        qWarning("err: %s", e.message().latin1());
        if (exc)
            delete exc;

        exc = new QXmlParseException(e.message());
    }
    QXmlParseException *exc;
};

class NotesFileParser : public QObject, public QXmlDefaultHandler
{
    Q_OBJECT

public:
    NotesFileParser();
    bool startDocument();
    bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes& );
    bool endElement(const QString&, const QString&, const QString&);

signals:
    void notesShowReminderAtStartUpLoaded(bool);
    void backupLoaded(bool, const QString &);
    void entryLoaded(Entry *);
    void securityPasswdLoaded(const QString &);
    void noteLoaded(const QString &, QList<QString> *, const QString &, const QString &, int, QDateTime, const QString &, QDateTime);
    void noteLoaded(const QString &, Strokes *, const QString &, const QString &, int, QDateTime, const QString &, QDateTime);
    void noNoteChild();

private:
    QString entryName;
    Entry *entry;
    bool inConfig;
};

#endif
