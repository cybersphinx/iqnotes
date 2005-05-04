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
#ifndef WRTIEDEFINEDENTRY_H
#define WRITEDEFINEDENTRY_H

#include "writeDefinedEntryBase.h"
#include "entry.h"
#include "notes.h"

#include <qlist.h>

class QVBoxLayout;
class QWidget;
class QString;

class WriteDefinedEntry : public WriteDefinedEntryBase
{
    Q_OBJECT

public:
    WriteDefinedEntry(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~WriteDefinedEntry();

    void setEntryName(const QString &name);

    void addOneLine(const QString &label, const PropertyData &pd);
    void addMultiLine(const QString &label, const PropertyData &pd);

    int exec();

    QList<PropertyData> *getDataList()
    {
        return data;
    }
    QList<PropertyStruct::EPropertyType> *getPropertyTypeList()
    {
        return propType;
    }

public slots:
    void propertyChanged(int index);
protected slots:
    virtual void accept();

private:
    QList<PropertyData> *data;
    QList<PropertyStruct::EPropertyType> *propType;
    uint lastSelectedProperty;
    PropertyStruct::EPropertyType lastTypeProperty;
};


#endif
