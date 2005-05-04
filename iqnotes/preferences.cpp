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

#include "preferences.h"
#include "md5.h"

#include <qapplication.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qtabwidget.h>
#include <qcheckbox.h>
#include <qdir.h>
#include <qfontdatabase.h>
#include <qfont.h>
#include <qcombobox.h>
#include <qstringlist.h>

PreferencesD::PreferencesD(QWidget* parent, const char* name, bool modal, WFlags fl) : PreferencesBase(parent, name, modal, fl)
{
    currentPasswd = "";

	// fonts
	QFontDatabase fontDB;
	QStringList fontFamilies = fontDB.families();
	currentFont = qApp->font();
	
	for (QStringList::Iterator it = fontFamilies.begin(); it != fontFamilies.end(); ++it)
	{
		Font->insertItem(*it);
		if ((*it).lower() == currentFont.family().lower())
			Font->setCurrentItem(Font->count() - 1);
	}

	updateSizes();
}

void PreferencesD::accept()
{
    if (SaveBackup->isChecked())
    {
        QDir backupDir(BackupLocation->text());
        
        if (!backupDir.exists())
        {
            TabWidget->showPage(BackupTab);
            BackupLocation->setFocus();
            QMessageBox::critical(this, "Invalid directory", "Please, enter the existing\ndirectory in backup location.", 0, 0);
            return;
        }
    }
    
    if (Passwd1->text().length() || Passwd2->text().length() || DontEncrypt->isChecked())
    {
        if (currentPasswd.length())
        {
            MD5 md5passwd(CurrentPasswd->text());

            if (currentPasswd != md5passwd.hex_digest())
            {
                QMessageBox::critical(this, "Bad password", "Please, enter the valid current\npassword.", 0, 0);
                return;
            }
        }

        if (DontEncrypt->isChecked())
        {
            Passwd1->setText("");
            Passwd2->setText("");

            PreferencesBase::accept();
            return;
        }

        if (Passwd1->text() == Passwd2->text())
        {
            PreferencesBase::accept();
            return;
        }
        else
        {
            TabWidget->showPage(SecurityTab);
            Passwd1->setFocus();
            QMessageBox::critical(this, "Passwords doesn't match", "Please, enter the same passwords.", 0, 0);
            return;
        }
    }

    PreferencesBase::accept();
}

void PreferencesD::updateSizes()
{
	QFontDatabase fontDB;
	int currentSize = -1;
#ifdef DEBUG
    int f = 1;
#else
    int f = 10;
#endif

	if (Font->currentText().lower() == currentFont.family().lower())
		currentSize = currentFont.pointSize();

	FontSize->clear();
	QValueList<int> sizes = fontDB.pointSizes(Font->currentText());
	for(QValueList<int>::Iterator it = sizes.begin(); it != sizes.end(); it++)
	{
		FontSize->insertItem(QString::number(*it/f));
		if (currentSize != -1 && (int)((*it)/f) == currentSize)
			FontSize->setCurrentItem(FontSize->count() - 1);
	}
}
