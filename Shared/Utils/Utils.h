#ifndef TCS_UTILS_UTILS_H
#define TCS_UTILS_UTILS_H

#include <QString>
#include <QColor>
#include <QPair>

#define FILE_PREFIX QString("file:///")

namespace TCS {
namespace Utils {

typedef QPair<qint8, qint8> IntPair ;
typedef QPair<qreal, qreal> FloatPair;

QString formatTime(qint64 ms);

int getLastSlashInd(const QString &path);


QString dataDir(const QString &fileName = QString());
QString logDir(const QString& filename = QString());
QString findUniqueLogFilename(const QString& prefix);
QString findUniqueFilename(const QString& dir, const QString& prefix, const QString& ext);
void removeFilesFromDataDir(const QStringList& filters);

QString makeValidUrl(const QString &iUrl);

QColor InverseColor(const QColor& c);

quint32 calcWaveFormatChunkLength(const QString &fileName);

bool lunchProgram(const QString& filename, const QStringList& args);


} // Utils
} // TCS

#endif // TCS_UTILS_UTILS_H
