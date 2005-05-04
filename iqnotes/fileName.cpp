#include "fileName.h"

#include <qfile.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpe/global.h>

FileName::FileName(QWidget* parent, const char* name, bool modal, WFlags fl) :
        FileNameBase(parent, name, modal, fl)
{}

void FileName::accept()
{
    QFile decr(Global::applicationFileName("iqnotes", FileNameI->text() + ".xml"));
    QFile encr(Global::applicationFileName("iqnotes", FileNameI->text() + ".rijn"));

    if (decr.exists() || encr.exists())
    {
        QMessageBox::warning(this, tr("Warning"), tr("This file exists.\nPlease choose another name."), 0, 0, 0);

        return;
    }

    FileNameBase::accept();
}
