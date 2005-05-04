/****************************************************************************
** Form implementation generated from reading ui file 'iqnotesAlarmBase.ui'
**
** Created: Sun May 11 15:53:00 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "iqnotesAlarmBase.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a IQNotesAlarmBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
IQNotesAlarmBase::IQNotesAlarmBase( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "IQNotesAlarmBase" );
    resize( 196, 234 ); 
    setCaption( tr( "IQNotes Alarm" ) );
    IQNotesAlarmBaseLayout = new QVBoxLayout( this ); 
    IQNotesAlarmBaseLayout->setSpacing( 6 );
    IQNotesAlarmBaseLayout->setMargin( 11 );

    AlarmText = new QLabel( this, "AlarmText" );
    AlarmText->setText( tr( "" ) );
    AlarmText->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter | QLabel::AlignLeft ) );
    IQNotesAlarmBaseLayout->addWidget( AlarmText );

    // signals and slots connections
}

/*  
 *  Destroys the object and frees any allocated resources
 */
IQNotesAlarmBase::~IQNotesAlarmBase()
{
    // no need to delete child widgets, Qt does it all for us
}

void IQNotesAlarmBase::receive(const QCString &, const QByteArray &)
{
    qWarning( "IQNotesAlarmBase::receive(const QCString &, const QByteArray &): Not implemented yet!" );
}

