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
#include "noteType.h"

#include <qlistbox.h>

NoteType::NoteType(QWidget* parent, const char* name, bool modal, WFlags fl)
        : NoteTypeBase(parent, name, modal, fl)
{}

void NoteType::addNoteType(const QString &type)
{
    NoteTypeList->insertItem(type);
}

void NoteType::addNoteType(const QPixmap &pm, const QString &type)
{
    NoteTypeList->insertItem(pm, type);
}