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
#include "sortTasks.h"

#include "qcombobox.h"

SortTasks::EType SortTasks::getOrder()
{
    if (OrderCombo->currentItem() == 0)
        return ORDER_ASCEND;
    else
        return ORDER_DESCEND;
}

SortTasks::EType SortTasks::getNonTasks()
{
    switch(NonTasksCombo->currentItem())
    {
    case 0:
        return NON_TASKS_FIRST;
    case 1:
        return NON_TASKS_LAST;
    }
}

SortTasks::EType SortTasks::getSortType()
{
    switch(SortTypeCombo->currentItem())
    {
    case 0:
        return SORT_CURRENT_LEVEL;
    case 1:
        return SORT_CURRENT_AND_DOWN;
    case 2:
        return SORT_WHOLE_TREE;
    }
}
