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
#include "writeDefinedEntry.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qlist.h>
#include <qstring.h>
#include <qcombobox.h>

WriteDefinedEntry::WriteDefinedEntry(QWidget* parent, const char* name, bool modal, WFlags fl)
        : WriteDefinedEntryBase(parent, name, modal, fl)
{
    data = new QList<PropertyData>;
    propType = new QList<PropertyStruct::EPropertyType>;
    propType->setAutoDelete(true);
    lastSelectedProperty = 0;
}

WriteDefinedEntry::~WriteDefinedEntry()
{
    delete data;
    delete propType;
}

int WriteDefinedEntry::exec()
{
    lastTypeProperty = *(propType->at(0));
    lastSelectedProperty = 0;
    propertyChanged(0);

    return WriteDefinedEntryBase::exec();
}

void WriteDefinedEntry::accept()
{
    // Save last modified property
    data->at(lastSelectedProperty)->setData(lastTypeProperty == PropertyStruct::ONE_LINE ? OneLine->text() : MultiLine->text());

    WriteDefinedEntryBase::accept();
}

void WriteDefinedEntry::setEntryName(const QString &name)
{
    EntryName->setText("<b>" + name + "</b>");
}

void WriteDefinedEntry::addOneLine(const QString &label, const PropertyData &pd)
{
    Properties->insertItem(label);
    data->append(new PropertyData(pd));

    PropertyStruct::EPropertyType *pt = new PropertyStruct::EPropertyType;
    *pt = PropertyStruct::ONE_LINE;
    propType->append(pt);

    // First property
    if (data->count() == 1)
        OneLine->setText(pd.getData());
}

void WriteDefinedEntry::addMultiLine(const QString &label, const PropertyData &pd)
{
    Properties->insertItem(label);
    data->append(new PropertyData(pd));

    PropertyStruct::EPropertyType *pt = new PropertyStruct::EPropertyType;
    *pt = PropertyStruct::MULTI_LINE;
    propType->append(pt);

    // First property
    if (data->count() == 1)
        MultiLine->setText(pd.getData());
}

void WriteDefinedEntry::propertyChanged(int index)
{
    if (*(propType->at(index)) == PropertyStruct::ONE_LINE)
    {
        data->at(lastSelectedProperty)->setData(lastTypeProperty == PropertyStruct::ONE_LINE ? OneLine->text() : MultiLine->text());
        lastSelectedProperty = index;
        lastTypeProperty = PropertyStruct::ONE_LINE;

        MultiLine->setDisabled(true);
        MultiLine->setText("*disabled*");

        OneLine->setEnabled(true);
        OneLine->setText(data->at(index)->getData());
	OneLine->setFocus();
    }
    else
    {
        data->at(lastSelectedProperty)->setData(lastTypeProperty == PropertyStruct::ONE_LINE ? OneLine->text() : MultiLine->text());
        lastSelectedProperty = index;
        lastTypeProperty = PropertyStruct::MULTI_LINE;

        OneLine->setDisabled(true);
        OneLine->setText("*disabled*");

        MultiLine->setEnabled(true);
        MultiLine->setText(data->at(index)->getData());
	MultiLine->setFocus();
    }
}
