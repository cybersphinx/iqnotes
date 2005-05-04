/****************************************************************************
** IQNotesAlarmBase meta object code from reading C++ file 'iqnotesAlarmBase.h'
**
** Created: Sun May 11 15:53:01 2003
**      by: The Qt MOC ($Id: qt/src/moc/moc.y   2.3.3   edited 2001-10-17 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "iqnotesAlarmBase.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *IQNotesAlarmBase::className() const
{
    return "IQNotesAlarmBase";
}

QMetaObject *IQNotesAlarmBase::metaObj = 0;

void IQNotesAlarmBase::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QDialog::className(), "QDialog") != 0 )
	badSuperclassWarning("IQNotesAlarmBase","QDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString IQNotesAlarmBase::tr(const char* s)
{
    return qApp->translate( "IQNotesAlarmBase", s, 0 );
}

QString IQNotesAlarmBase::tr(const char* s, const char * c)
{
    return qApp->translate( "IQNotesAlarmBase", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* IQNotesAlarmBase::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (IQNotesAlarmBase::*m1_t0)(const QCString&,const QByteArray&);
    typedef void (QObject::*om1_t0)(const QCString&,const QByteArray&);
    m1_t0 v1_0 = &IQNotesAlarmBase::receive;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    QMetaData *slot_tbl = QMetaObject::new_metadata(1);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(1);
    slot_tbl[0].name = "receive(const QCString&,const QByteArray&)";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"IQNotesAlarmBase", "QDialog",
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    metaObj->set_slot_access( slot_tbl_access );
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    return metaObj;
}
