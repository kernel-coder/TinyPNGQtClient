#ifndef TCS_DATA_QTAPPDATASETTINGS_H
#define TCS_DATA_QTAPPDATASETTINGS_H

#include <QObject>
#include <QVariant>


#define QADS TCS::Data::QtAppDataSettings::me()

class QWidget;

namespace TCS {
namespace Data {

struct QtAppDataSettingsPri;

class QtAppDataSettings : public QObject
{
    QtAppDataSettings(QObject* p = 0);
    ~QtAppDataSettings();

public:
    static QtAppDataSettings* me();
    void beginGroup(const QString& groupName);
    void endGroup();
    void setValue(const QString&  key, const QVariant&  value);
    QVariant value(const QString&  key, const QVariant&  defaultvalue = QVariant());
    void saveWidgetState(QWidget* w);
    void restoreWidgetState(QWidget* w);

private:
    QtAppDataSettingsPri*  d;
};

} // Data
} // TCS
#endif // TCS_DATA_QTAPPDATASETTINGS_H
