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
#include "defineEntry.h"
#include "app.h"
#include "entry.h"
#include "choosePic.h"

#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qlist.h>

#include <qpe/resource.h>

extern App *IQApp;

PropertyBoxItem::PropertyBoxItem(QListBox * listbox, const QString & text, EPropertyType pt)
        : QListBoxText(listbox, text)
{
    propertyType = pt;
    propertyID = 0;
}

PropertyBoxItem::PropertyBoxItem(PropertyStruct *ps) : QListBoxText(0, ps->getName())
{
    propertyType = ps->getType() == PropertyStruct::ONE_LINE ? ONE_LINE : MULTI_LINE;
    propertyID = ps->getID();

#ifdef DEBUG

    qDebug("PropertyBoxItem::PropertyBoxItem(PropertyStruct *ps) -- name: %s id: %d", ps->getName().latin1(), propertyID);
#endif
}

DefineEntry::DefineEntry(QWidget* parent, const char* name, bool modal, WFlags fl)
        : DefineEntryBase(parent, name, modal, fl)
{
    disableWidgets();
    propertyNChanged = false;
    editEntry = 0;
    isUsed = false;
}

void DefineEntry::addProperty(PropertyBoxItem *pbi)
{
    EntryProperties->insertItem(pbi);
    enableWidgets();
}

void DefineEntry::setEditEntry(Entry *ee)
{
    editEntry = ee;
    defaultPic = editEntry->getDefaultPic();
}

void DefineEntry::addNewProperty()
{
    PropertyBoxItem *pbi;

    EntryProperties->insertItem(pbi = new PropertyBoxItem(0, "New Property"));
    EntryProperties->setCurrentItem(pbi);
    enableWidgets();
    propertySelectedChange(pbi);
}

void DefineEntry::deleteProperty()
{
#ifdef DEBUG
    qDebug("void DefineEntry::deleteProperty()");
#endif

    if (isUsed)
    {
        if (QMessageBox::warning(this, "Delete", "By deleting this property, you\nwill delete all data associated\nwith this property in the nodes of\nthis type.\nAre you sure??", 1, 2, 0) != 1)
            return;

        int deletedID = static_cast<PropertyBoxItem *>(EntryProperties->item(EntryProperties->currentItem()))->getID();
        if (deletedID)
            deletedProperties.append(deletedID);
#ifdef DEBUG
        qDebug("deleted property id: %d", deletedID);
#endif

    }

    EntryProperties->setFocus();
    EntryProperties->removeItem(EntryProperties->currentItem());

    if (!EntryProperties->count())
    {
        PropertyName->setText("");
        disableWidgets();
    }
}

void DefineEntry::accept()
{
#ifdef DEBUG
    qDebug("DefineEntry::accpet()");
#endif
    if (!EntryProperties->count())
    {
        QMessageBox::warning(this, "No properties", "Please, define some entry's\nproperties", 0, 0, 0);
        return;
    }

    if (EntryName->text() == "")
    {
        QMessageBox::warning(this, "No entry name", "Please, define entry name", 0, 0, 0);
        return;
    }

    Entries *entries = IQApp->entries();

    Entry *entry;

    if (editEntry)
    {
        if (EntryName->text() != editEntry->getName() && entries->isIn(EntryName->text()))
        {
            QMessageBox::warning(this, "Entry exists", "Entry with such name\nexists. Please, choose another.", 0, 0, 0);
            return;
        }

        entry = editEntry;
        entry->clear();
        entry->setName(EntryName->text());
    }
    else if (entry = entries->isIn(EntryName->text()))
    {
        QMessageBox::warning(this, "Entry exists", "Entry with such name\nexists. Please, choose another.", 0, 0, 0);
        return;
    }
    else
        entry = new Entry(EntryName->text());

    for (uint i = 0; true; i++)
    {
        PropertyBoxItem *pbi;

        if (pbi = static_cast<PropertyBoxItem *>(EntryProperties->item(i)))
        {
            entry->addProperty(new PropertyStruct(pbi));
        }
        else
            break;
    }
    
    entry->setDefaultPic(defaultPic);

    if (!editEntry)
        entries->addEntry(entry);

    entry->checkPropertiesID();

    DefineEntryBase::accept();
}

void DefineEntry::propertySelectedChange(QListBoxItem *lbi)
{
    if (!EntryProperties->count())
    {
        return;
    }

    PropertyBoxItem *pbi;
    pbi = static_cast<PropertyBoxItem *>(lbi);

    int currentItemIndex = EntryProperties->currentItem();
    if (currentItemIndex == 0)
        MoveUp->setEnabled(false);
    else
        MoveUp->setEnabled(true);
    if (currentItemIndex == EntryProperties->count() - 1)
        MoveDown->setEnabled(false);
    else
        MoveDown->setEnabled(true);

    if (!PropertyName->hasFocus())
    {
        PropertyName->setText(pbi->text());

        setInputType(pbi);
    }
}

void DefineEntry::setPropertyOneLine()
{
    qWarning("one line");
    static_cast<PropertyBoxItem *>(EntryProperties->item(EntryProperties->currentItem()))->setType(PropertyBoxItem::ONE_LINE);
}

void DefineEntry::setPropertyMultiLine()
{
    qWarning("multi line");
    static_cast<PropertyBoxItem *>(EntryProperties->item(EntryProperties->currentItem()))->setType(PropertyBoxItem::MULTI_LINE);
}

void DefineEntry::setInputType(QListBoxItem *lvi)
{
    PropertyBoxItem *pbi;
    pbi = static_cast<PropertyBoxItem *>(lvi);

    if (pbi->getType() == PropertyBoxItem::ONE_LINE)
    {
        InputTypeGroup->setButton(0);
    }
    else
    {
        InputTypeGroup->setButton(1);
    }
}

void DefineEntry::propertyNameChanged(const QString &text)
{
    if (EntryProperties->count() && PropertyName->hasFocus())
    {
        PropertyBoxItem *pbi;
        (pbi = static_cast<PropertyBoxItem *>(EntryProperties->item(EntryProperties->currentItem())))->changeText(text);
        EntryProperties->changeItem(new PropertyBoxItem(0, pbi->text(), pbi->getType()), EntryProperties->currentItem());
    }
}

void DefineEntry::moveUp()
{
    int currentItemIndex = EntryProperties->currentItem();
    QListBoxItem *li = EntryProperties->item(currentItemIndex);
    EntryProperties->takeItem(li);

    EntryProperties->insertItem(li, currentItemIndex - 1);
    EntryProperties->setCurrentItem(li);
}

void DefineEntry::moveDown()
{
    int currentItemIndex = EntryProperties->currentItem();
    QListBoxItem *li = EntryProperties->item(currentItemIndex);
    EntryProperties->takeItem(li);

    EntryProperties->insertItem(li, currentItemIndex + 1);
    EntryProperties->setCurrentItem(li);
}

void DefineEntry::disableWidgets()
{
    InputTypeGroup->setEnabled(false);
    DeleteProperty->setEnabled(false);
    PropertyName->setEnabled(false);
    PropertyNameLabel->setEnabled(false);

    MoveUp->setEnabled(false);
    MoveDown->setEnabled(false);
}

void DefineEntry::enableWidgets()
{
    InputTypeGroup->setEnabled(true);
    DeleteProperty->setEnabled(true);
    PropertyName->setEnabled(true);
    PropertyNameLabel->setEnabled(true);

    MoveUp->setEnabled(true);
    MoveDown->setEnabled(true);
}

void DefineEntry::setDefaultPic()
{
    ChoosePic cp(this, 0, true);
    
    if (defaultPic != "")
	cp.setCurrentPic(defaultPic);
    
    cp.showMaximized();
    if (cp.exec())
    {
	if (cp.picFile() != "")
	{
	    DefaultPic->setPixmap(*(cp.pixmap()));
	    defaultPic = cp.picFile();
	}
        else
        	DefaultPic->setText("Defaul Pic");
    }
}

int DefineEntry::exec()
{
    if (editEntry && editEntry->getDefaultPic() != "")
	DefaultPic->setPixmap(Resource::loadPixmap("iqnotes/items/" + editEntry->getDefaultPic()));
    
    return DefineEntryBase::exec();
}
