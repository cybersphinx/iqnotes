#ifndef OPENFILE_H
#define OPENFILE_H

#include "openFileBase.h"

#include <qlistbox.h>

class OpenFile : public OpenFileBase
{
    Q_OBJECT
        
public:
    OpenFile(bool showNewFile_, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    
    QString getSelectedFileName() const { return FileList->currentText(); }
    int getSelectedIndex() const { return FileList->currentItem(); }

signals:
    void fileSelected(int, const QString &);

protected slots:
    virtual void accept();

private:
    bool showNewFile;
};

#endif
