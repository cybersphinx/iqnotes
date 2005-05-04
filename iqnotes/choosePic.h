#ifndef CHOOSEPICBASE_H
#define CHOOSEPIVBASE_H

#include "choosePicBase.h"

class QPixmap;

class ChoosePic : public ChoosePicBase
{
public:
    ChoosePic(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~ChoosePic();
    
    void setCurrentPic(const QString &cp);

    const QPixmap *pixmap();
    QString picFile();

private:
    QStringList picNames;
};

#endif
