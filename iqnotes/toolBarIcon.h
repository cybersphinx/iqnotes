/*  IQNotes - Smarty notes
    Copyright (C) 2001,2002,2003,2004 Peter Vrabel <kybu@kybu.org>
    
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
#include <qpixmap.h>
#include <qimage.h>
#include <qapplication.h>
#include <qpe/resource.h>

class ToolBarIcon
{
public:
    static QPixmap prepare(QString const &name)
    {
        QString n = name;
        
        if (qApp->desktop()->width() >= 480)
            n += "_big";
        
        return Resource::loadPixmap(n);
    }
};
