#include "choosePic.h"
#include <qdir.h>
#include <qstringlist.h>
#include <qpixmap.h>
#include <qlistbox.h>
#include <qfileinfo.h>

ChoosePic::ChoosePic(QWidget* parent, const char* name, bool modal, WFlags fl)
        : ChoosePicBase(parent, name, modal, fl)
{
#ifdef DESKTOP
    QDir d(QString(IQNOTES_PICDIR) + "/iqnotes/items");
#else
    QDir d(QString("/opt/QtPalmtop/pics") + "/iqnotes/items");
#endif
    picNames = d.entryList();

    PicsList->insertItem("none");
    for(QStringList::Iterator it = picNames.begin(); it != picNames.end(); ++it )
    {
        QString fName = *it;
        QFileInfo fInfo(fName);

        QString fListName = fName.left(fName.length() - fInfo.extension().length() - 1);
        
#ifdef DESKTOP
        PicsList->insertItem(QPixmap(QString(IQNOTES_PICDIR) + "/iqnotes/items/" + *it), fListName);
#else
        PicsList->insertItem(QPixmap(QString("/opt/QtPalmtop/pics") + "/iqnotes/items/" + *it), fListName);
#endif
    }

    PicsList->setCurrentItem(0);
}

ChoosePic::~ChoosePic()
{}

void ChoosePic::setCurrentPic(const QString &cp)
{
#if 0
    if (PicsList->count() <= 1)
	return;
 
    int i = 1;
    for(QStringList::Iterator it = picNames.begin(); it != picNames.end(); ++it )
    {
	if (*it == cp) 
	{
	    PicsList->setSelected(i, true);
	    break;
	}
	i++;
    }

#endif
    for (int i=0 ; i < PicsList->count() ; i++) {
        if (PicsList->text(i) == cp) {
            PicsList->setSelected(i, true);
            break;
        }
    }
}

const QPixmap *ChoosePic::pixmap()
{
    int idx;
    if ((idx = PicsList->currentItem()) != 0)
        return PicsList->pixmap(idx);

    return 0;
}

QString ChoosePic::picFile()
{
    int idx;
    if ((idx = PicsList->currentItem()) != 0)
//        return picNames[idx - 1];
        return PicsList->text(idx);

    return "";
}
