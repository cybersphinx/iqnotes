#include <qregexp.h>
#include "noteDetail.h"
#include "app.h"
#include "notes.h"

extern App *IQApp;

NoteDetail::NoteDetail(QWidget *parent)
    : QWidget(parent)
{
    formatedText = new QTextView(this, "noteData");
    text = new QMultiLineEdit(this);
    text->setReadOnly(true);
    sketch = new QSketch(this);
    sketch->setReadOnly();
    sketch->setMoveMode(true);
}

const QString &NoteDetail::getFormatedData()
{
    if (!noteData->getEntryName().length())
        return noteData->getTextData();

    // Set the entry
    Entries *entries = IQApp->entries();
    Entry *entry = entries->isIn(noteData->getEntryName());

    PropertyStruct *ps;
    QString *formatedData = new QString;
    int i = 0;

    for (ps = entry->first(); ps; ps = entry->next())
    {
#ifdef DEBUG
        qDebug("property name: %s id: %d", ps->getName().latin1(), ps->getID());
#endif

        if (!noteData->getPropertyData(ps->getID())->getData().length())
        {
#ifdef DEBUG
            qDebug("skipped");
#endif
            i++;
            continue;
        }

        if (ps->isOneLine())
            *formatedData += "<b>" + ps->getName() + ":</b> " +
                             noteData->getPropertyData(ps->getID())->getData() + "<br>";
        else
            *formatedData += "<b>" + ps->getName() + ":</b><br>" +
                             QString(noteData->getPropertyData(ps->getID())->getData()).replace(
                             QRegExp("\n"), "<br>") + "<br>";

        i++;
    }

    return *formatedData;
}

void NoteDetail::setNoteData(NoteData *value)
{
    noteData = value;
    setUpdatesEnabled(false);
    if (noteData->isText())
    {
        text->setText(getFormatedData());
        text->show();
        sketch->hide();
        formatedText->hide();
    }
    else if (noteData->isFormatedText())
    {
        formatedText->setTextFormat(Qt::RichText);
        formatedText->setText(getFormatedData());
        text->hide();
        sketch->hide();
        formatedText->show();
    }
    else
    {
        sketch->setStrokes(value->getStrokes());
        text->hide();
        sketch->show();
        formatedText->hide();
    }
    setUpdatesEnabled(true);
    repaint();
}

void NoteDetail::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    formatedText->resize(e->size());
    text->resize(e->size());
    sketch->resize(e->size());
}

void NoteDetail::paintEvent(QPaintEvent *e)
{
    text->repaint();
    formatedText->repaint();
    sketch->repaint();
}

void NoteDetail::setWordWrap(bool wrap)
{
    if (wrap)
	{
        text->setWordWrap(QMultiLineEdit::WidgetWidth);
		text->setWrapPolicy(QMultiLineEdit::Anywhere);
	}
    else
        text->setWordWrap(QMultiLineEdit::NoWrap);
}
