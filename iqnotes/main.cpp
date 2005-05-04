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
#include "app.h"
#include <qpe/qpeapplication.h>

int main(int argc, char ** argv)
{
    QPEApplication a(argc, argv);

    App mw;

#ifdef DEMO
	mw.openTree(-2, "");
#else
	mw.openFile(true);
#endif
	a.showMainWidget(&mw);
    return a.exec();
}
