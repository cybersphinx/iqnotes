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

#include "iqnotesAlarmBase.h"

#include <qlabel.h>
#include <qdatetime.h>
#include <qcstring.h>
#include <qdatastream.h>
#include <qpe/qpeapplication.h>

class IQNotesAlarm : public IQNotesAlarmBase
{
public:
	IQNotesAlarm(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
	
public slots:
	virtual void receive(const QCString &, const QByteArray &);
};

IQNotesAlarm::IQNotesAlarm(QWidget* parent, const char* name, bool modal, WFlags fl) :
	IQNotesAlarmBase(parent, name, modal, fl)
{
	connect(qApp, SIGNAL(appMessage(const QCString &, const QByteArray &)), this, SLOT(receive(const QCString &, const QByteArray &)));
}

void IQNotesAlarm::receive(const QCString& msg, const QByteArray& data)
{
	QDataStream stream(data, IO_ReadOnly);
	QDateTime when;
	stream >> when;

	AlarmText->setText(msg);
}

int main(int argc, char ** argv)
{
    QPEApplication a(argc, argv);
    
    IQNotesAlarm mw;

	a.showMainWidget(&mw);
    int r = a.exec();
}
