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

#include "writeSketchNote.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qiconset.h>
#include <qpixmap.h>

WriteSketchNote::WriteSketchNote(QWidget* parent, const char* name, bool modal, WFlags fl) :
        QDialog(parent, name, modal, fl)
{
    vlayout = new QVBoxLayout(this);
    vlayout->setSpacing(0);
    vlayout->setMargin(0);

    hlayout = new QHBoxLayout(this);
    hlayout->setSpacing(4);
    hlayout->setMargin(11);
    vlayout->addLayout(hlayout);

    //
    // Buttons
    chooseWidth = new QPushButton("Width", this);
    hlayout->addWidget(chooseWidth);


    sketch = new QSketch(this);
	//    sketch->setMaximumSize(238, 119 + 119);
    vlayout->addWidget(sketch);


    QPixmap *px = new QPixmap(7, 7);
    px->fill(sketch->getCurrentColor());
    chooseColorB = new QPushButton(QIconSet(*px), "Color", this);
    hlayout->addWidget(chooseColorB);

    deleteStroke = new QPushButton("Delete", this);
    hlayout->addWidget(deleteStroke);

    moveB = new QPushButton("Move", this);
    moveB->setToggleButton(true);
    hlayout->addWidget(moveB);

    connect(chooseWidth, SIGNAL(clicked()), sketch, SLOT(chooseWidth()));
    connect(chooseColorB, SIGNAL(clicked()), this, SLOT(chooseColor()));
    connect(deleteStroke, SIGNAL(clicked()), sketch, SLOT(deleteLastStroke()));
	connect(moveB, SIGNAL(toggled(bool)), sketch, SLOT(setMoveMode(bool)));
	
#ifdef DESKTOP
	hlayout2 = new QHBoxLayout(this);
	hlayout2->setSpacing(4);
	hlayout2->setMargin(11);
	vlayout->addLayout(hlayout2);
	QPushButton *okB = new QPushButton("&OK", this);
	QPushButton *cancelB = new QPushButton("&Cancel", this);
	connect(okB, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelB, SIGNAL(clicked()), this, SLOT(reject()));
	hlayout2->addWidget(cancelB);
	hlayout2->addWidget(okB);
#endif

    setCaption(tr("Sketch"));
}

WriteSketchNote::~WriteSketchNote()
{}

void WriteSketchNote::chooseColor()
{
    QPixmap *px = new QPixmap(7, 7);
    sketch->chooseColor();
    px->fill(sketch->getCurrentColor());

    chooseColorB->setIconSet(QIconSet(*px));

    delete px;
}
