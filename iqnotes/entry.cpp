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
#include "entry.h"

#include <qlist.h>

// {{{ Entry

Entry::Entry(const QString &name1)
{
    propertyID = 0;
    propList = new QList<PropertyStruct>();
    propList->setAutoDelete(true);
    setName(name1);
    defaultPic = "";
}

Entry::~Entry()
{
    delete propList;
}

void Entry::clear()
{
    delete propList;
    propList = new QList<PropertyStruct>();
    name = "";
    propertyID = 0;
}

int Entry::getFreePropertyID()
{
    int freeID = 0;
    bool in;
    do
    {
        freeID++;

        QListIterator<PropertyStruct> it(*propList);
        in = false;
        for (; it.current(); ++it)
        {
            if ((it.current())->getID() == freeID)
            {
                in = true;
                break;
            }
        }
    }
    while (in);

    return freeID;
}

bool Entry::addProperty(PropertyStruct *es)
{
    if (!isIn(es))
    {
#ifdef DEBUG
        qDebug("Entry::addProperty -- Property Name: %s Property ID: %d", es->getName().latin1(), es->getID());
#endif

        propList->append(es);

        return true;
    }

    return false;
}

bool Entry::isIn(PropertyStruct *es)
{
	/*
    for (PropertyStruct *es1 = propList->first(); es1; es1 = propList->next())
    {
      if (es1->getName() == es->getName())
        return true;
    }
	*/
	
    return false;
}

void Entry::checkPropertiesID()
{
    QListIterator<PropertyStruct> it(*propList);

#ifdef DEBUG

    qDebug("Entry::checkPropertiesID()");
#endif

    for (; it.current(); ++it)
    {
        if (!it.current()->getID())
            it.current()->setID(getFreePropertyID());
#ifdef DEBUG

        qDebug("name: %s id: %d", it.current()->getName().latin1(), it.current()->getID());
#endif

    }
}

// }}}

// {{{ Entries

Entries::Entries()
{
    entryList = new QList<Entry>();
    entryList->setAutoDelete(true);
}

Entries::~Entries()
{
    delete entryList;
}

bool Entries::addEntry(Entry *e)
{
    if (!isIn(e))
    {
        entryList->append(e);
        return true;
    }

    return false;
}

Entry *Entries::isIn(Entry *e)
{
    for (Entry *e1 = entryList->first(); e1; e1 = entryList->next())
    {
        if (e1->getName() == e->getName())
            return e1;
    }

    return 0;
}

Entry *Entries::isIn(const QString &name)
{
    for (Entry *e1 = entryList->first(); e1; e1 = entryList->next())
    {
        if (e1->getName() == name)
            return e1;
    }

    return 0;
}

bool Entries::remove(const QString &name)
{
    for (Entry *e1 = entryList->first(); e1; e1 = entryList->next())
    {
        if (e1->getName() == name)
        {
            return entryList->remove
                   (e1);
        }
    }

    return false;
}

// }}}

QString Entry::getDefaultPic()
{
    return defaultPic;
}


void Entry::setDefaultPic(const QString &dp)
{
    defaultPic = dp;
}
