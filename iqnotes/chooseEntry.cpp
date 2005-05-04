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
#include "chooseEntry.h"
#include "entry.h"

#include <qmessagebox.h>
#include <qpe/resource.h>

ChooseEntry::ChooseEntry(QWidget* parent, const char* name, bool modal, WFlags fl)
        : ChooseEntryBase(parent, name, modal, fl)
{
    entriesList = 0;
}

void ChooseEntry::fillList(Entries *entriesL)
{
    entriesList = entriesL;

    Entry *entry;
    for (entry = entriesList->first(); entry; entry = entriesList->next())
    {
	if (entry->getDefaultPic() == "")
	    EntriesBox->insertItem(entry->getName());
	else
	    EntriesBox->insertItem(Resource::loadPixmap("iqnotes/items/" + entry->getDefaultPic()), entry->getName());
    }
}

void ChooseEntry::canDeleteEntry(const QString &entryName, bool deleteIt)
{
    if (!deleteIt)
    {
        QMessageBox::warning(this, tr("Warning"), tr("This entry is used in notes.\nIt can't be changed."), 0, 0, 0);
        return;
    }

    int currItem = EntriesBox->currentItem();
    EntriesBox->removeItem(currItem);
}


void ChooseEntry::deleteEntry()
{
    emit deleteEntry(EntriesBox->currentText());
}
