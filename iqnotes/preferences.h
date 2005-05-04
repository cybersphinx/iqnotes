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
#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "preferencesBase.h"
#include <qfont.h>

class PreferencesD : public PreferencesBase
{
public:
    PreferencesD(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);

    void setCurrentPasswd(const QString &passwd)
    {
        currentPasswd = passwd;
    }
protected slots:
    virtual void accept();
 
public slots:
    virtual void updateSizes(); 

private:
    QString currentPasswd;
	QFont currentFont;
};

#endif
