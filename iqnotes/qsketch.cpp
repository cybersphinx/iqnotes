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

#include "qsketch.h"
#include "colorDialog.h"

#include <qpainter.h>
#include <qbrush.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <qcolordialog.h>

// {{{ Stroke

Stroke::Stroke()
{
    init(0, QWidget::black);
}

Stroke::Stroke(int width, QColor color)
{
    init(width, color);
}

void Stroke::init(int width, QColor color)
{
    pen.setWidth(width);
    pen.setColor(color);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);

    points.setAutoDelete(true);

    maxX = maxY = 0;
}

void Stroke::addPoint(const QPoint point)
{
    if (maxX < point.x())
        maxX = point.x();
    if (maxY < point.y())
        maxY = point.y();
    
    QPoint *p = new QPoint(point);
    points.append(p);
}

void Stroke::paint(QPainter *p, const QPoint &drift, bool onlyLastPoint)
{
    const QPen &oldPen = p->pen();
    p->setPen(pen);

    if (points.count() == 1 && !onlyLastPoint)
    {
        QPoint *point = points.getFirst();

        p->drawEllipse((point->x() - drift.x()) - int(pen.width()/2), (point->y() - drift.y()) - int(pen.width()/2), pen.width(), pen.width());
    }
    // paint all points
    else if (!onlyLastPoint)
    {
        for (QPoint *point = points.first(); (point = points.current()); points.next())
        {
            // It's first point
            if (point == points.getFirst())
                p->moveTo(QPoint(point->x() - drift.x(), point->y() - drift.y()));

            p->lineTo(QPoint(point->x() - drift.x(), point->y() - drift.y()));
        }
    }
    // paint only last point
    else
    {
        QPoint *last = points.getLast(), *prev = points.prev();

        if (prev)
        {
            p->moveTo(QPoint(prev->x() - drift.x(), prev->y() - drift.y()));
            p->lineTo(QPoint(last->x() - drift.x(), last->y() - drift.y()));
        }
    }

    p->setPen(oldPen);
}

QString Stroke::serialize()
{
    QString r;
    r.sprintf("%d,%d,%d,%d", pen.width(), pen.color().red(), pen.color().green(), pen.color().blue());

    for (QPoint *point = points.first(); (point = points.current()); points.next())
    {
        QString p;
        r += p.sprintf(",%d,%d", point->x(), point->y());
    }

    return r;
}

void Stroke::unserialize(QString data)
{
    QString p;
    int idx = 0, red, green, blue;
    QPoint point;
    QColor color;

    points.clear();

    for (int i = 0; i < data.length(); i++)
    {
        if (!(data[i] == ','))
        {
            p += data[i];
        }
        else
        {
            // width
            if (idx == 0)
            {
                pen.setWidth(p.toInt());
                p = "";
                idx++;
            }
            // red
            else if (idx == 1)
            {
                red = p.toInt();
                p = "";
                idx++;
            }
            // green
            else if (idx == 2)
            {
                green = p.toInt();
                p = "";
                idx++;
            }
            // blue
            else if (idx == 3)
            {
                blue = p.toInt();
                p = "";
                idx++;
            }
            // X
            else if ((idx % 2) == 0)
            {
                point.setX(p.toInt());
                p = "";
                idx++;
            }
            // Y
            else
            {
                point.setY(p.toInt());
                p = "";
                idx++;

                addPoint(point);
            }
        }
    }

    point.setY(p.toInt());
    addPoint(point);

    color.setRgb(red, green, blue);
    pen.setColor(color);
}

// }}}

// {{{ Strokes

Strokes::Strokes()
{
    maxX = maxY = 0;
    currentStroke = 0;
    strokes.setAutoDelete(true);
}

Strokes::Strokes(Strokes &strks)
{
    strokes.clear();
    for (Stroke *stroke = strks.first(); (stroke = strks.current()); strks.next())
    {
        QString d = stroke->serialize();
        addStroke(stroke->serialize());
    }

	maxX = strks.getMaxX();
	maxY = strks.getMaxY();

    currentStroke = 0;
}

Strokes::~Strokes()
{
    closeStroke();
}

void Strokes::openStroke(int width, QColor color)
{
    currentStroke = new Stroke(width, color);
}

void Strokes::addPoint(const QPoint point)
{
    if (!currentStroke)
        return;

    if (point.x() > maxX)
        maxX = point.x();
    if (point.y() > maxY)
        maxY = point.y();
    
    currentStroke->addPoint(point);
}

void Strokes::closeStroke()
{
    if (currentStroke)
        strokes.append(currentStroke);

    currentStroke = 0;
}

void Strokes::paint(QPainter *p, const QPoint &drift)
{
    for (Stroke *stroke = strokes.first(); (stroke = strokes.current()); strokes.next())
    {
        stroke->paint(p, drift);
    }

    if (currentStroke)
        currentStroke->paint(p, drift);
}

void Strokes::removeLastStroke()
{
    // recalculate the max values
    Stroke *last = 0;
    if ((last = strokes.last()) && (maxX == last->getMaxX() || maxY == last->getMaxY()))
    {
        maxX = maxY = 0;
        for (Stroke *stroke = strokes.first(); (stroke = strokes.current()); strokes.next())
        {
            if (stroke == last)
                break;
            
            if (maxX < stroke->getMaxX())
                maxX = stroke->getMaxX();
            if (maxY < stroke->getMaxY())
                maxY = stroke->getMaxY();
        }
    }
    
    strokes.removeLast();
}

QString Strokes::serialize()
{
    QString r;
    uint i = 0;

    for (Stroke *stroke = strokes.first(); (stroke = strokes.current()); strokes.next(), i++)
    {
        QString p;

        r += p.sprintf("stroke%d=\"", i) + stroke->serialize() + "\"\n";
    }

    return r;
}

void Strokes::addStroke(const QString &serialData)
{
    Stroke *stroke = new Stroke();

    stroke->unserialize(serialData);
    strokes.append(stroke);

	if (stroke->getMaxX() > maxX)
		maxX = stroke->getMaxX();
	if (stroke->getMaxY() > maxY)
		maxY = stroke->getMaxY();
	
}

// }}}

// {{{ QSketch

QSketch::QSketch(QWidget *parent, const char *name) :
        QWidget(parent, name)
{
    viewP = QPoint(0, 0);
    viewCache = sketchCache = 0;
    readOnly = false;
    modified = false;
    mouseDown = false;
	moveMode = false;

    strokeWidth = 2;
    currentColor = black;

    strokes = new Strokes;
    externalStrokes = false;

	//	updatePicCacheSize();

    setFocusPolicy(QWidget::StrongFocus);

    choosedColors.setAutoDelete(true);
	// background color
	choosedColors.append(new QColor(colorGroup().base()));
    choosedColors.append(new QColor(0, 0, 0));
}

QSketch::~QSketch()
{
	if (viewCache)
		delete viewCache;
	if (sketchCache)
		delete sketchCache;
}

void QSketch::resizeEvent(QResizeEvent *re)
{
	if (moveMode)
	{
		updateViewCache();
	}
		
	/*
	int i;
	if (viewP.x() > (i = picCache->width() - width() + style().defaultFrameWidth()))
		viewP.setX(i);
	if (viewP.y() > (i = picCache->height() - height() + style().defaultFrameWidth()))
		viewP.setY(i);
	*/
}

void QSketch::paintEvent(QPaintEvent *pe)
{
#ifdef DEBUG
	qDebug("QSketch::paintEvent");
#endif
	
	QPainter pt(this);
	
	const QColorGroup &cg = colorGroup();
	QBrush brush = cg.brush(QColorGroup::Base);

	if (moveMode)
	{
		if (modified)
		{
			updateViewCache();
			modified = false;
		}
		//pt.drawPixmap(0, 0, *viewCache);
		bitBlt(this, 0, 0, viewCache);

		return;
	}

	if (strokes)
	{
		if (pe->erased())
		{
			// background
			pt.fillRect(0, 0, width(), height(), brush);
			strokes->paint(&pt, viewP);
		}
		else if (mouseDown)
		{
			if (Stroke *cs = strokes->getCurrentStroke())
				cs->paint(&pt, viewP, true);
		}
		else
		{
			if (strokes->getLastStroke())
				strokes->getLastStroke()->paint(&pt, viewP);
		}
	}

	// frame
	style().drawPanel(&pt, 0, 0, width(), height(), cg, true, style().defaultFrameWidth());

    modified = false;
}

void QSketch::setStrokes(Strokes *strokes1)
{
    if (strokes && !externalStrokes)
        delete strokes;

    // colors used in the strokes put into choosedColors dialog
    for (Stroke *stroke = strokes1->first(); (stroke = strokes1->current()); strokes1->next())
    {
        QColor strokeColor = stroke->getColor();
        bool colorIn = false;
        for(QColor *color = choosedColors.first(); (color = choosedColors.current()); choosedColors.next())
        {
            if (*color == strokeColor)
            {
                colorIn = true;
                break;
            }
        }

        if (!colorIn)
        {
            choosedColors.append(new QColor(strokeColor));
        }
    }

    strokes = strokes1;
    externalStrokes = true;

	viewP = QPoint(0, 0);

	// rebuild cache
	if (moveMode)
	{
		if (viewCache)
			delete viewCache;
		if(sketchCache)
			delete sketchCache;
		viewCache = sketchCache = 0;

		updateViewCache();
		update();
	}

    modified = true;
}

void QSketch::clearStrokes() {
	if (strokes && !externalStrokes)
        delete strokes;

	strokes = new Strokes;
    externalStrokes = false;

	if (sketchCache)
		delete sketchCache;
	sketchCache = 0;

	modified = true;
	repaint();
}

void QSketch::setMoveMode(bool move)
{
    moveMode = move;
	
	if (moveMode)
		updateViewCache();
	else
	{
		delete viewCache;
		delete sketchCache;
		viewCache = 0;
		sketchCache = 0;
	}
}

void QSketch::mousePressEvent(QMouseEvent *e)
{
    mouseDownStart = e->pos();

    if (readOnly)
        return;
    if (moveMode)
        return;

	QPoint p = e->pos() + viewP;
    
    modified = true;
    mouseDown = true;
    strokes->openStroke(strokeWidth, currentColor);
    strokes->addPoint(p);
}

void QSketch::mouseReleaseEvent(QMouseEvent *e)
{
    if (readOnly)
        return;
    if (moveMode)
        return;

    modified = true;
    mouseDown = false;
    strokes->closeStroke();
	repaint(false);;
}

void QSketch::mouseMoveEvent(QMouseEvent *e)
{
	if (moveMode)
	{
		int i;
		QPoint d = e->pos() - mouseDownStart;

		viewP += d;
		if (viewP.x() < 0 || !strokes || readOnly && strokes && strokes->getMaxX() < width())
			viewP.setX(0);
		else if (!readOnly && viewP.x() > (i = strokes->getMaxX() + style().defaultFrameWidth()))
			viewP.setX(i);
		else if (readOnly && viewP.x() > (i = strokes->getMaxX() - width() + style().defaultFrameWidth() * 2))
			viewP.setX(i);
		
		if (viewP.y() < 0 || !strokes || readOnly && strokes && strokes->getMaxY() < height())
			viewP.setY(0);
		else if (!readOnly && strokes && viewP.y() > (i = strokes->getMaxY() + style().defaultFrameWidth()))
			viewP.setY(i);
		else if (readOnly && viewP.y() > (i = strokes->getMaxY() - height() + style().defaultFrameWidth() * 2))
			viewP.setY(i);
		
		
#ifdef DEBUG
		qDebug("QSketch::mouseMoveEvent viewX=%d viewY=%d", viewP.x(), viewP.y());
#endif
		modified = true;
		repaint(false);
		
		return;
	}
	
    if (readOnly)
        return;

	QPoint p = e->pos() + viewP;

    modified = true;
    strokes->addPoint(p);
	repaint(false);
}

void QSketch::keyPressEvent(QKeyEvent *e)
{
    /*
    if (readOnly)
    return;

    // Erase last stroke    
    if (e->key() == Qt::Key_Left)
    {
    deleteLastStroke();
    }
    // Stroke width
    else if (e->key() == Qt::Key_Up)
    {
    chooseWidth();
    } 
    */
}

void QSketch::chooseWidth()
{
    if (mouseDown)
        return;

    QPopupMenu popup(this);

    const int width1 = popup.insertItem("Stroke width: 1");
    const int width2 = popup.insertItem("Stroke width: 2");
    const int width3 = popup.insertItem("Stroke width: 3");
    const int width4 = popup.insertItem("Stroke width: 4");

    // Disable current stroke width
    switch(strokeWidth)
    {
    case 1:
        popup.setItemEnabled(width1, false);
        break;
    case 2:
        popup.setItemEnabled(width2, false);
        break;
    case 3:
        popup.setItemEnabled(width3, false);
        break;
    case 4:
        popup.setItemEnabled(width4, false);
        break;
    }

    QSize popSize = popup.sizeHint();
    int selectedWidth = popup.exec(QPoint(int((240 - popSize.width())/2), int((320 - popSize.height())/2)));

    if (selectedWidth == width1)
        strokeWidth = 1;
    else if (selectedWidth == width2)
        strokeWidth = 2;
    else if (selectedWidth == width3)
        strokeWidth = 3;
    else if (selectedWidth == width4)
        strokeWidth = 4;
}

void QSketch::chooseColor()
{
    if (mouseDown)
        return;

    QPopupMenu popup(this);
    QPixmap *px;

	//
    // fill popup menu
    int i = 0;
	//
    for (QColor *color = choosedColors.first(); color; color = choosedColors.next())
    {
        px = new QPixmap(10, 10);
        px->fill(*color);
        popup.insertItem(QIconSet(*px), "", i++);
        delete px;
    }
    int customID = popup.insertItem("Custom");

    QSize popSize = popup.sizeHint();
    int selectedColor = popup.exec(QPoint(int((240 - popSize.width())/2), int((320 - popSize.height())/2)));

    if (selectedColor == -1)
        return;

    // custom color
    if (selectedColor == customID)
    {
        QColor *color;
        if (choosedColors.count() != 1)
            color = new QColor(ColorDialog::getColor(*(choosedColors.getLast())));
        else
            color = new QColor(ColorDialog::getColor(QColor(255, 255, 255)));


        if (!color->isValid())
        {
            delete color;
            return;
        }

        choosedColors.append(color);
        currentColor = *color;
    }
    else
    {
        currentColor = *(choosedColors.at(selectedColor));
    }
}

void QSketch::deleteLastStroke()
{
    if (!mouseDown)
    {
        modified = true;
        strokes->removeLastStroke();
        repaint(true);
    }
}

void QSketch::updateViewCache()
{
	if (!viewCache)
		viewCache = new QPixmap(size());
	else if (viewCache->size() != size())
		viewCache->resize(size());

	QPainter pv(viewCache);
	const QColorGroup &cg = colorGroup();
	QBrush brush = cg.brush(QColorGroup::Base);

	if (!sketchCache && strokes->getMaxX() && strokes->getMaxY())
	{
		sketchCache = new QPixmap(strokes->getMaxX(), strokes->getMaxY());
		QPainter ps(sketchCache);

		// background
		ps.fillRect(0, 0, sketchCache->width(), sketchCache->height(), brush);
		strokes->paint(&ps);
	}
	
	// background
	pv.fillRect(0, 0, width(), height(), brush);
	if (sketchCache)
		bitBlt(viewCache, 0, 0, sketchCache, viewP.x(), viewP.y(), viewCache->width(), viewCache->height());
	// frame
	style().drawPanel(&pv, 0, 0, width(), height(), cg, true, style().defaultFrameWidth());
}

// }}}

