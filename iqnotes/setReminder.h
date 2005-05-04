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
#ifndef SETREMINDER_H
#define SETREMINDER_H

#include "setReminderBase.h"
#include <qlineedit.h>

class QDateTime;

class SetReminder : public SetReminderBase
{
public:
    SetReminder(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);

    QDateTime getDateTime();
    void setDateTime(QDateTime dt);

	void setAlarm(bool alarmStatus);

    int exec();

public slots:
    virtual void ignoreTimeClicked();
	virtual void alarmClicked(); 
};

#endif
