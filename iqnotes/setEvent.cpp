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
#include "setEvent.h"

#include <qvalidator.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qspinbox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qcheckbox.h>

SetEvent::SetEvent(QWidget* parent, const char* name, WFlags fl)
        : SetEventBase(parent, name, fl)
{
    QIntValidator *iv = new QIntValidator(1, 31, this);
    QIntValidator *iv1 = new QIntValidator(0, 23, this);
    QIntValidator *iv2 = new QIntValidator(0, 59, this);

    Day->setValidator(iv);
    Hour->setValidator(iv1);
    Min->setValidator(iv2);

    QDateTime currentDT = QDateTime::currentDateTime();
    QString p;

    p.sprintf("%d", currentDT.date().day());
    Day->setText(p);
    Month->setCurrentItem(currentDT.date().month() - 1);
    Year->setValue(currentDT.date().year());

    p.sprintf("%d", currentDT.time().hour());
    Hour->setText(p);
    p.sprintf("%d", currentDT.time().minute());
    Min->setText(p);
}

QDateTime SetEvent::getDateTime()
{
    QDateTime dt;

    dt.setDate(QDate(Year->value(), Month->currentItem() + 1, Day->text().toInt()));
    if (!IgnoreTime->isChecked())
        dt.setTime(QTime(Hour->text().toInt(), Min->text().toInt()));
    else
        dt.setTime(QTime(25, 65)); // invalid time, time is ignored

    return dt;
}

void SetEvent::setDateTime(QDateTime dt)
{
    QDateTime currentDT = dt;
    QString p;

    p.sprintf("%d", currentDT.date().day());
    Day->setText(p);
    Month->setCurrentItem(currentDT.date().month() - 1);
    Year->setValue(currentDT.date().year());

    p.sprintf("%d", currentDT.time().hour());
    Hour->setText(p);
    p.sprintf("%d", currentDT.time().minute());
    Min->setText(p);
}

int SetEvent::exec()
{
    int r;
    do
    {
        r = SetEventBase::exec();
        if (r)
        {
            if ((!IgnoreTime->isChecked() && !getDateTime().isValid()) ||
                    (IgnoreTime->isChecked() && !getDateTime().date().isValid()))
            {
                QMessageBox::warning(this, "Datetime is invalid", "Please, enter the valid datetime.");
            }
            else
                return r;
        }
        else
            return r;
    }
    while (1);
}

void SetEvent::ignoreTimeClicked()
{
    if (IgnoreTime->isChecked())
    {
        Hour->setEnabled(false);
        Min->setEnabled(false);
        Glue->setEnabled(false);
    }
    else
    {
        Hour->setEnabled(true);
        Min->setEnabled(true);
        Glue->setEnabled(true);
    }
}
