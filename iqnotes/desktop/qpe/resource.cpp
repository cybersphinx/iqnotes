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

#include "resource.h"
#include <qpixmap.h>

QPixmap Resource::loadPixmap(const QString &name)
{
#ifdef DEBUG
	qDebug("Resource::loadPixmap");
	qDebug(QString(IQNOTES_PICDIR) + name);
#endif

	return QPixmap(QString(IQNOTES_PICDIR) + "/" + name + ".png");
}
