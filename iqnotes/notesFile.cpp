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

#include "entry.h"
#include "notesFile.h"
#include "qsketch.h"

#include <qxml.h>


NotesFileParser::NotesFileParser() : QXmlDefaultHandler()
{
    entryName = "";
    entry = 0;
    inConfig = false;
}

bool NotesFileParser::startDocument()
{
    return true;
}

bool NotesFileParser::startElement(const QString &, const QString &, const QString &qName, const QXmlAttributes &attr)
{
    if (qName == "config")
    {
        inConfig = true;
    }
    else if (qName == "backup" && inConfig)
    {
        emit backupLoaded(attr.value("save") == "yes" ? true : false, attr.value("location"));
    }
    else if (qName == "notes" && inConfig)
    {
        emit notesShowReminderAtStartUpLoaded(attr.value("showreminder") == "no" ? false : true);
    }
    else if (qName == "security" && inConfig)
    {
        emit securityPasswdLoaded(attr.value("passwd"));
    }
    else if (qName == "entry")
    {
        entryName = attr.value("name");
        entry = new Entry(entryName);
	entry->setDefaultPic(attr.value("defaultpic"));
    }
    else if (qName == "property" && entry)
    {
        PropertyStruct *ps = new PropertyStruct(attr.value("name"),
                                                attr.value("type") == "oneLine" ? PropertyStruct::ONE_LINE : PropertyStruct::MULTI_LINE);
        entry->addProperty(ps);
    }
    else if (qName == "note")
    {
        QList<QString> *data = new QList<QString>;
        Strokes *strokes = new Strokes;
        QString en, n, per, day, month, year, hour, min, pic = "",
            rday, rmonth, ryear, rhour, rmin;
        int startIdx = 2, prio = 0;
        QDateTime dt, rem;

        n = attr.value("name");
        per = attr.value("percentage");

        // It's task
        if (per.length())
        {
            prio = attr.value("priority").toInt();
            startIdx += 2;
        }

        day = attr.value("day");
        // It's event
        if (day.length())
        {
            month = attr.value("month");
            year = attr.value("year");
            startIdx += 3;

            dt.setDate(QDate(year.toInt(), month.toInt(), day.toInt()));

            hour = attr.value("hour");
            if (hour.length())
            {
                min = attr.value("min");
                startIdx += 2;

                dt.setTime(QTime(hour.toInt(), min.toInt()));
            }
            else
                dt.setTime(QTime(25, 61));

        }
        
        rday = attr.value("rday");
        // has reminder
        if (rday.length())
        {
            rmonth = attr.value("rmonth");
            ryear = attr.value("ryear");
            startIdx += 3;

            rem.setDate(QDate(ryear.toInt(), rmonth.toInt(), rday.toInt()));

            rhour = attr.value("rhour");
            if (rhour.length())
            {
                rmin = attr.value("rmin");
                startIdx += 2;

                rem.setTime(QTime(rhour.toInt(), rmin.toInt()));
            }
            else
                rem.setTime(QTime(25, 61));

        }

        // Has pic
        pic = attr.value("pic");
        if (pic.length())
        {
            startIdx += 1;
        }

        // Text note
        if ((en = attr.value("type")) == "Text")
        {
            delete strokes;

            en = "";
            data->append(new QString(attr.value("data0")));

            emit noteLoaded(n, data, en, per, prio, dt, pic, rem);
        }
        // Sketch note
        else if (en == "Sketch")
        {
            delete data;

            for (int i = startIdx; i < attr.length(); i++)
            {
                strokes->addStroke(attr.value(i));
            }

            emit noteLoaded(n, strokes, en, per, prio, dt, pic, rem);
        }
        // Defined note
        else
        {
            delete strokes;

            for (int i = startIdx; i < attr.length(); i++)
            {
                data->append(new QString(attr.value(i)));
            }

            emit noteLoaded(n, data, en, per, prio, dt, pic, rem);
        }
    }

    return true;
}

bool NotesFileParser::endElement(const QString&, const QString&, const QString& qName)
{
    if (qName == "entry")
    {
        entry->checkPropertiesID();
        emit entryLoaded(entry);

        entryName = "";
        entry = 0;
    }
    else if (qName == "config")
    {
        inConfig = 0;
    }
    else if (qName == "note")
    {
        emit noNoteChild();
    }

    return true;
}
