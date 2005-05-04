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
#ifndef QSKETCH_H
#define QSKETCH_H

#include <qpixmap.h>
#include <qwidget.h>
#include <qlist.h>
#include <qpen.h>
#include <qlist.h>
#include <qpoint.h>

// {{{ Stroke

class Stroke
{
public:
    Stroke();
    Stroke(Stroke &s) { unserialize(s.serialize()); }
    Stroke(int width, QColor color);
    void addPoint(const QPoint point);

    void paint(QPainter *p, const QPoint &drift = QPoint(0, 0), bool onlyLastPoint = false);

    QString serialize() ;
    void unserialize(QString data);

    uint getWidth() { return pen.width(); }
    QColor getColor() { return pen.color(); }

    int getMaxX() { return maxX; }
    int getMaxY() { return maxY; }

private:
    void init(int width, QColor color);

    QList<QPoint> points;
    QPen pen;

    int maxX, maxY;
};

// }}}

// {{{ Strokes

class Strokes
{
public:
    Strokes();
    Strokes(Strokes &strks);
    ~Strokes();

    int getMaxX() { return maxX; }
    int getMaxY() { return maxY; }

    // open new stroke
    void openStroke(int width, QColor color);
    // add point to current stroke
    void addPoint(const QPoint point);
    // close current stroke
    void closeStroke();
    //
    void removeLastStroke();

    QString serialize();
    void addStroke(const QString &serialData);

    // opened stroke
    Stroke *getCurrentStroke() { return currentStroke; }
    // closed stroke
    Stroke *getLastStroke() { return strokes.getLast(); }

    Stroke *first() { return strokes.first(); }
    Stroke *next() { return strokes.next(); }
    Stroke *current() { return strokes.current(); }

    void paint(QPainter *p, const QPoint &drift = QPoint(0, 0));

private:
    QList<Stroke> strokes;
    Stroke *currentStroke;

    int maxX, maxY;
};

// }}}

// {{{ QSketch

class QSketch : public QWidget
{
    Q_OBJECT

public:
    QSketch(QWidget *parent=0, const char *name=0);
    ~QSketch();
    
    // Object doesn't delete strokes!!!!!!!!
    Strokes *getStrokes() { return strokes; }
    void setStrokes(Strokes *strokes1);
	void clearStrokes();

    void setReadOnly() { readOnly = true; }

    QColor getCurrentColor() const { return currentColor; }
    int getCurrentWidth() const { return strokeWidth; }

public slots:
    void chooseWidth();
    void chooseColor();
    void deleteLastStroke();
	void setMoveMode(bool);

protected:
    void paintEvent(QPaintEvent *pe);
    void resizeEvent(QResizeEvent *re);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);

private:
	void updateViewCache();
	
    QPixmap *viewCache, *sketchCache;
    Strokes *strokes;
    bool mouseDown, readOnly, externalStrokes, modified, moveMode;
	QPoint mouseDownStart;

    int strokeWidth;
	QPoint viewP;
    QColor currentColor;
    QList<QColor> choosedColors;
};

// }}}

#endif

/*
  Local Variables:
  mode: c++
  End:
*/
