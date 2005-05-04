#ifndef FILENAME_H
#define FILENAME_H

#include "fileNameBase.h"

class FileName : public FileNameBase
{
public:
    FileName(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);

protected slots:
    virtual void accept();
};

#endif
