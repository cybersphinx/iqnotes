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

#ifndef WRITESKETCHNOTE_H
#define WRITESKETCHNOTE_H

#include <qdialog.h>
#include "qsketch.h"

class QSketch;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;

class WriteSketchNote : public QDialog
{
    Q_OBJECT

public:
    WriteSketchNote(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~WriteSketchNote();

    Strokes *getStrokes()
    {
        return sketch->getStrokes();
    }
    void setStrokes(Strokes *strokes)
    {
        sketch->setStrokes(strokes);
    }

    QSketch *sketch;
    QVBoxLayout *vlayout;
    QHBoxLayout *hlayout;
    QPushButton *chooseWidth, *chooseColorB, *deleteStroke, *moveB;

public slots:
    void chooseColor();
};

#endif
