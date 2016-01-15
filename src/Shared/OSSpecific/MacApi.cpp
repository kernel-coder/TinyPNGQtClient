#ifdef __APPLE__
#include "osapi.h"
#include <QDebug>
#include <QProcess>
#include <QApplication>


QString OSApi::AppDir()
{
    return QApplication::applicationDirPath() + "/../../../";
}


QString OSApi::ResourcesDir()
{
    return QApplication::applicationDirPath() + "/../Resources/";
}


void OSApi::PrintFileTo(QString printer, QString file)
{
    QStringList args;args<<"-d"<<printer<<file;
    qDebug() << "Executing command: lp " << args.join(' ');
    QProcess::startDetached("lp", args);
}

void OSApi::ExecuteInstallerSilently(const QString& filename)
{
    //http://stackoverflow.com/questions/21428208/how-to-silently-install-dmg-file-in-mac-os-x-system-using-shell-script
    QProcess::startDetached(ResourcesDir() + "install_app.sh", filename);
}

#endif
