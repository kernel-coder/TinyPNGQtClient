#ifdef __linux__
#include "osapi.h"
#include <QProcess>
#include <QDebug>
#include <QApplication>


QString OSApi::AppDir()
{
    return qApp->applicationDirPath() + QString("/");
}


QString OSApi::ResourcesDir()
{
    return qApp->applicationDirPath() + QString("/");
}


void OSApi::PrintFileTo(QString printer, QString file)
{
    QStringList args;args<<"-d"<<printer<<file;
    qDebug() << "Executing command: lp " << args.join(' ');
    QProcess::startDetached("lp", args);
}

void OSApi::ExecuteInstallerSilently(const QString& filename)
{
    //http://askubuntu.com/questions/40779/how-do-i-install-a-deb-file-via-the-command-line
    QProcess::startDetached(ResourcesDir() + "install_app.sh", filename);
}

#endif
