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
#ifndef SORT_H
#define SORT_H

#include "sortBase.h"

// TODO: rewrite
#define TASK 0
#define EVENT 1
#define OTHER 2

class SortControl
{
public:
    enum OrderType { ASCEND, DESCEND, DONT_TOUCH };
    enum TasksSortBy { PERCENTAGE, PRIORITY };
    enum EventsWithoutTime { FIRST, LAST };
    enum SortType { CURRENT_LEVEL, CURRENT_LEVEL_DOWN, WHOLE_TREE };

    SortControl()
    {
        tasksOrder = DESCEND;
        tasksSortBy = PERCENTAGE;
        eventsOrder = ASCEND;
        eventsWithoutTime = FIRST;
        othersOrder = ASCEND;
        firstO = TASK;
        secondO = EVENT;
        thirdO = OTHER;
        sortType = CURRENT_LEVEL;
    }
    SortControl(const SortControl &sc)
    {
        *this = sc;
    }

    void setTasksOrder(OrderType ot)
    {
        tasksOrder = ot;
    }
    void setEventsOrder(OrderType ot)
    {
        eventsOrder = ot;
    }
    void setOthersOrder(OrderType ot)
    {
        othersOrder = ot;
    }

    void setTasksSortBy(TasksSortBy sb)
    {
        tasksSortBy = sb;
    }
    void setEventsWithoutTime(EventsWithoutTime wt)
    {
        eventsWithoutTime = wt;
    }

    void setFirst(int nt)
    {
        firstO = nt;
    }
    void setSecond(int nt)
    {
        secondO = nt;
    }
    void setThird(int nt)
    {
        thirdO = nt;
    }

    void setSortType(SortType st)
    {
        sortType = st;
    }

    OrderType getTasksOrder() const
    {
        return tasksOrder;
    }
    OrderType getEventsOrder() const
    {
        return eventsOrder;
    }
    OrderType getOthersOrder() const
    {
        return othersOrder;
    }

    TasksSortBy getTasksSortBy() const
    {
        return tasksSortBy;
    }
    EventsWithoutTime getEventsWithoutTime() const
    {
        return eventsWithoutTime;
    }

    int first() const
    {
        return firstO;
    }
    int second() const
    {
        return secondO;
    }
    int third() const
    {
        return thirdO;
    }

    SortType getSortType() const
    {
        return sortType;
    }

    SortControl &operator=(const SortControl &sc)
    {
        setTasksOrder(sc.getTasksOrder());
        setEventsOrder(sc.getEventsOrder());
        setOthersOrder(sc.getOthersOrder());

        setTasksSortBy(sc.getTasksSortBy());
        setEventsWithoutTime(sc.getEventsWithoutTime());

        setFirst(sc.first());
        setSecond(sc.second());
        setThird(sc.third());

        setSortType(sc.getSortType());

        return *this;
    }

private:
    OrderType tasksOrder, eventsOrder, othersOrder;
    TasksSortBy tasksSortBy;
    EventsWithoutTime eventsWithoutTime;
    int firstO, secondO, thirdO;
    SortType sortType;
};

class Sort : public SortBase
{
public:
    Sort(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);

    int exec();

    SortControl getSortControl();
    void setSortControl(const SortControl &sc);
};

#endif
