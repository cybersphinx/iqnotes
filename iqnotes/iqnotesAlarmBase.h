/****************************************************************************
** Form interface generated from reading ui file 'iqnotesAlarmBase.ui'
**
** Created: Sun May 11 15:52:58 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef IQNOTESALARMBASE_H
#define IQNOTESALARMBASE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;

class IQNotesAlarmBase : public QDialog
{ 
    Q_OBJECT

public:
    IQNotesAlarmBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~IQNotesAlarmBase();

    QLabel* AlarmText;

public slots:
    virtual void receive(const QCString &, const QByteArray &);

protected:
    QVBoxLayout* IQNotesAlarmBaseLayout;
};

#endif // IQNOTESALARMBASE_H
