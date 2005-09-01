#include "openFile.h"

#include <qdir.h>
#include <qstringlist.h>
#include <qfileinfo.h>

#include <qpe/global.h>

OpenFile::OpenFile(bool showNewFile_, QWidget* parent, const char* name, bool modal, WFlags fl) :
        OpenFileBase(parent, name, modal, fl)
{
    showNewFile = showNewFile_;
    
#ifdef DEBUG
    qDebug("OpenFile::OpenFile(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0)");
    qDebug("%s", Global::applicationFileName("iqnotes", "").latin1());
#endif

    QDir d(Global::applicationFileName("iqnotes", ""));
    d.setNameFilter("*.xml *.rijn");
    QStringList fileNames = d.entryList();

    if (showNewFile)
        FileList->insertItem("new data file");

    for(QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); ++it )
    {

#ifdef DEBUG
        qDebug("%s", (*it).latin1());
#endif

        QString fName = *it;
        QFileInfo fInfo(fName);

        QString fListName = fName.left(fName.length() - fInfo.extension().length() - 1);
        if (fListName == "#init_file#")
            continue;
        FileList->insertItem(fListName);
    }

    if (FileList->count())
        FileList->setCurrentItem(0);
}

void OpenFile::accept()
{
    int index = getSelectedIndex();
    if (showNewFile && !index)
        index = -2;
    emit fileSelected(index, getSelectedFileName());
    
    OpenFileBase::accept();
}
