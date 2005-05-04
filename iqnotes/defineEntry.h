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
#ifndef DEFINEENTRY_H
#define DEFINEENTRY_H

#include "defineEntryBase.h"
class PropertyStruct;

#include <qlistbox.h>
#include <qvaluelist.h>
class QListViewItem;
class Entry;

class PropertyBoxItem : public QListBoxText
{
public:
    enum EPropertyType {ONE_LINE, MULTI_LINE};
    PropertyBoxItem(QListBox * listbox = 0, const QString & text=QString::null, EPropertyType pt = ONE_LINE);
    PropertyBoxItem(PropertyStruct *ps);

    void setType(EPropertyType pt)
    {
        propertyType = pt;
    }
    EPropertyType getType()
    {
        return propertyType;
    }
    void setID(int id)
    {
        propertyID = id;
    }
    int getID() const
    {
        return propertyID;
    }
    void changeText(const QString &text)
    {
        setText(text);
    }

private:
    EPropertyType propertyType;
    int propertyID;
};

class DefineEntry : public DefineEntryBase
{
    Q_OBJECT

public:
    DefineEntry(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);

    void addProperty(PropertyBoxItem *pbi);
    void setEditEntry(Entry *ee);
    void setIsUsed(bool used)
    {
        isUsed = used;
    }
    bool getIsUsed() const
    {
        return isUsed;
    }

    QValueList<int> *getDeletedProperties()
    {
        return &deletedProperties;
    }
    
    int exec();

public slots:
    virtual void addNewProperty();
    virtual void deleteProperty();
    virtual void propertyNameChanged(const QString &);
    virtual void propertySelectedChange(QListBoxItem *);
    void setInputType(QListBoxItem *);

    virtual void setPropertyOneLine();
    virtual void setPropertyMultiLine();

    virtual void moveUp();
    virtual void moveDown();
    virtual void setDefaultPic();

protected slots:
    virtual void accept();

private:
    void disableWidgets();
    void enableWidgets();
    bool propertyNChanged, isUsed;

    Entry *editEntry;
    QValueList<int> deletedProperties;
    QString defaultPic;
};
#endif
