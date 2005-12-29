/*  IQNotes - Smarty notes
    Copyright (C) 2001, 2002 Peter Vrabel <kybu@kybu.org>
    
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
#ifndef NOTESCONFIG_H
#define NOTESCONFIG_H

class NotesConfig
{
 public:
    NotesConfig()
    {
        wordWrap = true;
        showScrollBars = true;
        rootNodeDecoration = false;
        verticalLayout = true;
        font = ""; fontSize = 0;
#ifdef DESKTOP
        desktopWarning = true;
#else
        desktopWarning = false;
#endif
    }

    NotesConfig &operator=(const NotesConfig &nc)
    {
        wordWrap = nc.wordWrap;
        showScrollBars = nc.showScrollBars;
        rootNodeDecoration = nc.rootNodeDecoration;
        verticalLayout = nc.verticalLayout;
        font = nc.font;
        fontSize = nc.fontSize;
        desktopWarning = nc.desktopWarning;
        
        return *this;
    }

    bool wordWrap, showScrollBars, rootNodeDecoration, verticalLayout,
         desktopWarning;
    QString font;
    int fontSize;
};


#endif
