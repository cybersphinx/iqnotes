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
#include "setTask.h"

#include <qcombobox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qfont.h>

SetTask::SetTask(QWidget* parent, const char* name, WFlags fl)
        : SetTaskBase(parent, name, fl)
{
    setPriority(2);
    setPercentage("");
    Percentage_valueChanged(0);  // since setPercentage might not move the slider

    setPercentage("");
}

#if 0
// no longer a typein, so no need to validate
int SetTask::exec()
{
    do
    {
        int r = SetTaskBase::exec();

        if (r)
        {
            int p = Percentage->value();

            if (p < 0 || p > 100)
                QMessageBox::warning(this, "Percentage", "Please, choose percentage between\n0 and 100");
            else
                return r;
        }
        else
            return r;
    }
    while (1);
}
#endif

void SetTask::setPercentage(const QString &per)
{
    Percentage->setValue( (per == "") ? 0 : per.toInt() );
}

void SetTask::setPriority(int prio)
{
    Priority->setCurrentItem(prio);
}

int SetTask::getPriority() const
{
    return Priority->currentItem();
}

// Overridden virtual slots

void SetTask::Percentage0_clicked()
{
    Percentage->setValue( 0 );
}

void SetTask::Percentage100_clicked()
{
    Percentage->setValue( 100 );
}

void SetTask::Percentage_sliderMoved(int newValue)
{
    //PercentageLabel->setText(QString("%1%").arg(newValue));
    Percentage_valueChanged(newValue);
}

void SetTask::Percentage_valueChanged(int newValue)
{
    PercentageLabel->setText(QString("%1%").arg(newValue));
}

