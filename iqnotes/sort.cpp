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
#include "sort.h"

#include <qcombobox.h>
#include <qradiobutton.h>
#include <qmessagebox.h>

Sort::Sort(QWidget* parent, const char* name, bool modal, WFlags fl)
        : SortBase(parent, name, modal, fl)
{}

SortControl Sort::getSortControl()
{
    SortControl sc;

    sc.setTasksOrder(TaskAscend->isChecked() ? SortControl::ASCEND : SortControl::DESCEND);
    sc.setEventsOrder(EventAscend->isChecked() ? SortControl::ASCEND : SortControl::DESCEND);
    sc.setOthersOrder(OtherAscend->isChecked() ? SortControl::ASCEND :
                      (OtherDescend->isChecked() ? SortControl::DESCEND : SortControl::DONT_TOUCH));

    sc.setTasksSortBy(TaskPercentage->isChecked() ? SortControl::PERCENTAGE : SortControl::PRIORITY);
    sc.setEventsWithoutTime(EventFirst->isChecked() ? SortControl::FIRST : SortControl::LAST);

    sc.setFirst(First->currentItem() == 0 ? TASK :
                (First->currentItem() == 1? EVENT : OTHER));
    sc.setSecond(Second->currentItem() == 0 ? TASK :
                 (Second->currentItem() == 1? EVENT : OTHER));
    sc.setThird(Third->currentItem() == 0 ? TASK :
                (Third->currentItem() == 1? EVENT : OTHER));

    sc.setSortType(SortType->currentItem() == 0 ? SortControl::CURRENT_LEVEL :
                   (SortType->currentItem() == 1 ? SortControl::CURRENT_LEVEL_DOWN : SortControl::WHOLE_TREE));

    return sc;
}

void Sort::setSortControl(const SortControl &sc)
{
    //
    // Tasks
    if (sc.getTasksOrder() == SortControl::ASCEND)
        TaskAscend->setChecked(true);
    else
        TaskDescend->setChecked(true);

    if (sc.getTasksSortBy() == SortControl::PERCENTAGE)
        TaskPercentage->setChecked(true);
    else
        TaskPriority->setChecked(true);

    //
    // Events
    if (sc.getEventsOrder() == SortControl::ASCEND)
        EventAscend->setChecked(true);
    else
        EventDescend->setChecked(true);

    if (sc.getEventsWithoutTime() == SortControl::FIRST)
        EventFirst->setChecked(true);
    else
        EventLast->setChecked(true);

    //
    // Others
    if (sc.getOthersOrder() == SortControl::ASCEND)
        OtherAscend->setChecked(true);
    else if (sc.getOthersOrder() == SortControl::DESCEND)
        OtherDescend->setChecked(true);
    else
        OtherDontTouch->setChecked(true);

    //
    // Setting
    First->setCurrentItem(sc.first());
    Second->setCurrentItem(sc.second());
    Third->setCurrentItem(sc.third());
    SortType->setCurrentItem(static_cast<int>(sc.getSortType()));
}

int Sort::exec()
{
    int r;
    do
    {
        r = SortBase::exec();

        if (r)
        {
            int f = First->currentItem(),
                    s = Second->currentItem(),
                        t = Third->currentItem();

            if (f == s || f == t ||
                    s == t)
                QMessageBox::warning(this, "Bad setting",
                                     "Please, correct the show order in\nSetting section. Each note\ntype can be choosen only once.");
            else
                return r;
        }
        else
            return r;

    }
    while (1);
}
