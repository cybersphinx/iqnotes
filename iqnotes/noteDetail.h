#ifndef NOTEDETAIL_H
#define NOTEDETAIL_H

#include <qwidget.h>
#include <qtextview.h>
#include <qsketch.h>
#include <qmultilineedit.h>

class NoteData;

class NoteDetail : public QWidget
{
    Q_OBJECT
    public:
        NoteDetail(QWidget *parent);
        void setNoteData(NoteData *value);
        void setWordWrap(bool wrap);

    protected:
        virtual void resizeEvent(QResizeEvent *e);
        virtual void paintEvent(QPaintEvent *e);

    private:
        const QString &getFormatedData();

        NoteData *noteData;
        QTextView *formatedText;
        QMultiLineEdit *text;
        QSketch *sketch;
};

 
#endif
