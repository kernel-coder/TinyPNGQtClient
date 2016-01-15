#include "Utils.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QProcess>
#include <QDebug>
#include <QDateTime>

namespace TCS {
namespace Utils {


QString formatTime(qint64 ms)
{
    qint32 minute = ms / (1000 * 60);
    ms = ms % (1000 * 60);
    qint32 second = ms / 1000;
    //qint32 msec = ms % 1000;
    QString formated("%1:%2");
    return formated.arg(minute, 2, 10, QChar('0')).arg(second, 2, 10, QChar('0'));
}


int getLastSlashInd(const QString &path)
{
    int iLastSlash = path.lastIndexOf('/');
    if (iLastSlash < 0)
        iLastSlash = path.lastIndexOf('\\');
    return iLastSlash;
}

/**
 * Returns the data directory, based on the OS's data
 * location, the company name and the application name.
 * If it doesn't exist, it is created
 * @param fileName A filename present in the data directory
 * @returns The absolute path of the data directory, otherwise
 *          if a filename is specified, it returns the abolute
 *          path of the filename in the data directory
 */
QString dataDir(const QString &fileName)
{
    QDir dd(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    if (!dd.exists())
        dd.mkpath(dd.absolutePath());

    if (fileName == "")
        return dd.absolutePath() + "/";
    else
        return dd.absoluteFilePath(fileName);
}


QString logDir(const QString &fileName)
{
    QDir dd(dataDir() + "Log");
    if (!dd.exists())
        dd.mkpath(dd.absolutePath());

    if (fileName == "")
        return dd.absolutePath() + "/";
    else
        return dd.absoluteFilePath(fileName);
}

QString findUniqueLogFilename(const QString& prefix)
{
    return findUniqueFilename(logDir(), prefix, "txt");
}

QString findUniqueFilename(const QString& dir, const QString& prefix, const QString& ext)
{
    int counter = 1;
    QString file = dir + QString("%1-%2.%3").arg(prefix).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss")).arg(ext);
    if (QFile::exists(file)) {
        file = dir + QString("%1-%2-%3.%4").arg(prefix).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss")).arg(counter).arg(ext);
        counter++;
    }
    return file;
}


void removeFilesFromDataDir(const QStringList &filters)
{
    QString dd = dataDir();
    QDir dir(dd);
    QStringList files = dir.entryList(filters, QDir::Files);
    foreach (QString file, files) {
        QFile::remove(dd + file);
    }
}


QString makeValidUrl(const QString &iUrl)
{
    QString oUrl = iUrl;
    if(!iUrl.startsWith("http:") && !iUrl.startsWith("\\\\")
            && !iUrl.startsWith(FILE_PREFIX))
        oUrl = FILE_PREFIX + iUrl;
    return oUrl;
}


QColor InverseColor(const QColor& c)
{
    return QColor(255 - c.red(), 255 - c.green(), 255 - c.blue());
}


bool lunchProgram(const QString &filename, const QStringList &args)
{
    return QProcess::startDetached(filename, args);
}

} // Utils
} // TCS
