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
#ifndef ENTRY_H
#define ENTRY_H

#include "defineEntry.h"

#include <qstring.h>
#include <qlist.h>

class PropertyStruct
{
public:
    enum EPropertyType {ONE_LINE, MULTI_LINE};
    PropertyStruct(PropertyBoxItem *pbi)
    {
        name = pbi->text();
        propertyType = pbi->getType() == PropertyBoxItem::ONE_LINE ? ONE_LINE : MULTI_LINE;
        id = pbi->getID();

#ifdef DEBUG
        qDebug("PropertyStruct::PropertyStruct(PropertyBoxItem *pbi) -- name: %s id: %d", pbi->text().latin1(), id);
#endif

    }

    PropertyStruct(const QString &name1, EPropertyType pt, int id1 = 0)
    {
        name = name1;
        propertyType = pt;
        id = id1;
    }

    const QString &getName()
    {
        return name;
    }
    EPropertyType getType()
    {
        return propertyType;
    }
    int getID() const
    {
        return id;
    }

    void setName(const QString &name1)
    {
        name = name1;
    }
    void setType(EPropertyType pt)
    {
        propertyType = pt;
    }
    void setID(int id1)
    {
        id = id1;
    }

    bool isOneLine()
    {
        return propertyType == ONE_LINE;
    }
    bool isMultiLine()
    {
        return propertyType == MULTI_LINE;
    }

private:
    QString name;
    int id;
    EPropertyType propertyType;
};

class Entry
{
public:
    Entry(const QString &name1);
    ~Entry();

    void clear();
    void setName(const QString &name1)
    {
        name = name1;
    }

    bool addProperty(PropertyStruct *es);
    bool isIn(PropertyStruct *es);
    void checkPropertiesID();

    const QString &getName()
    {
        return name;
    }

    PropertyStruct *first()
    {
        return propList->first();
    }
    PropertyStruct *next()
    {
        return propList->next();
    }
    PropertyStruct *prev()
    {
        return propList->prev();
    }
    PropertyStruct *last()
    {
        return propList->last();
    }
    PropertyStruct *at(uint i)
    {
        return propList->at(i);
    }

    int getFreePropertyID();
    QString getDefaultPic();
    void setDefaultPic(const QString &dp);

private:
    QString name;
    QList<PropertyStruct> *propList;
    int propertyID;
    QString defaultPic;
};

class Entries
{
public:
    Entries();
    ~Entries();

    uint count()
    {
        return entryList->count();
    }
    bool addEntry(Entry *e);
    Entry *isIn(Entry *e);
    Entry *isIn(const QString &name);

    Entry *first()
    {
        return entryList->first();
    }
    Entry *next()
    {
        return entryList->next();
    }
    Entry *prev()
    {
        return entryList->prev();
    }
    Entry *last()
    {
        return entryList->last();
    }
    Entry *at(uint i)
    {
        return entryList->at(i);
    }

    bool remove
        (const QString &name);
    bool remove
        (uint i)
    {
        return entryList->remove
               (i);
    }

    void clear()
    {
        entryList->clear();
    }

private:
    QList<Entry> *entryList;
};

#endif
