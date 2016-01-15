#include "QtAppDataSettings.h"
#include <QtCore>
#include <QDebug>
#include <QWidget>
#include <QDesktopWidget>
#include <QApplication>

namespace TCS {
namespace Data {

struct QtAppDataSettingsPri {
    QtAppDataSettingsPri() {}
    QSettings* Settings;
    static QtAppDataSettings* This;
};

QtAppDataSettings* QtAppDataSettingsPri::This = 0;

QtAppDataSettings::QtAppDataSettings(QObject*  p) : QObject(p), d(new QtAppDataSettingsPri)
{
    d->Settings = new QSettings(this);
}


QtAppDataSettings::~QtAppDataSettings() {delete d;}


QtAppDataSettings* QtAppDataSettings::me()
{
    if (QtAppDataSettingsPri::This == 0) {
        QtAppDataSettingsPri::This = new QtAppDataSettings(QCoreApplication::instance());
    }
    return QtAppDataSettingsPri::This;
}


void QtAppDataSettings::beginGroup(const QString& groupName)
{
    d->Settings->beginGroup(groupName);
}


void QtAppDataSettings::endGroup()
{
    d->Settings->endGroup();
}

void QtAppDataSettings::setValue(const QString &key, const QVariant &value)
{
    d->Settings->setValue(key, value);
}


QVariant QtAppDataSettings::value(const QString &key, const QVariant &defaultvalue)
{
    return d->Settings->value(key, defaultvalue);
}


void QtAppDataSettings::saveWidgetState(QWidget *w)
{
    setValue(QString("%1/size").arg(w->objectName()), w->size());
    setValue(QString("%1/pos").arg(w->objectName()), w->pos());
}


QPoint defaultWndPoint(QWidget* w)
{
    QDesktopWidget* dw = QApplication::desktop();
    int  x = (dw->width() - w->width())/2;
    int y = (dw->height() - w->height())/2;
    return QPoint(x, y);
}


void QtAppDataSettings::restoreWidgetState(QWidget *w)
{
    w->resize(value(QString("%1/size").arg(w->objectName()), w->size()).toSize());
    w->move(value(QString("%1/pos").arg(w->objectName()), defaultWndPoint(w)).toPoint());
}

} // Data
} // TCS
