#ifndef OSAPI_H
#define OSAPI_H

#include <QString>
#include <QStringList>

class OSApi
{
public:
    static QString AppDir();
    static QString ResourcesDir();
    static void PrintFileTo(QString printer, QString file);
    static void ExecuteInstallerSilently(const QString& filename);
};

#endif // OSAPI_H
