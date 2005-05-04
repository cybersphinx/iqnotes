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
#ifndef CHOOSEENTRY_H
#define CHOOSEENTRY_H

#include "chooseEntryBase.h"
#include <qlistbox.h>

class Entries;

class ChooseEntry : public ChooseEntryBase
{
    Q_OBJECT

public:
    ChooseEntry(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    void fillList(Entries *entriesL);

    int currentEntry()
    {
        return EntriesBox->currentItem();
    }

public slots:
    virtual void deleteEntry();
    void canDeleteEntry(const QString &, bool);

signals:
    void deleteEntry(const QString &);

private:
    Entries *entriesList;
};

#endif
