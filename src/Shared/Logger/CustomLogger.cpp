#include "customlogger.h"
#include <QApplication>
#include <QTimer>
#include <QDateTime>
#include "Utils/Utils.h"
#include <QDir>
#include <QDebug>

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    CustomLogger* logger = CustomLogger::me();
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        if (logger) logger->log("Debug", context, msg);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        if (logger) logger->log("Info", context, msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        if (logger) logger->log("Critical", context, msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        if (logger) logger->log("Fatal", context, msg);
        abort();
    }
    fflush(stderr);
}

static CustomLogger* sLoggerInstace = 0;

void appCleanup()
{
    if (sLoggerInstace) {
        delete sLoggerInstace;
        sLoggerInstace = 0;
    }
}


CustomLogger* CustomLogger::me()
{    
    if (sLoggerInstace == 0) {sLoggerInstace = new CustomLogger(qApp);}
    return sLoggerInstace;
}


CustomLogger::CustomLogger(QObject *p) : QObject(p)
{
}


CustomLogger::~CustomLogger() 
{
	saveLogsToFile(); 
	sLoggerInstace = 0;
}


void CustomLogger::log(const QString &prefix, const QMessageLogContext &context, const QString &msg)
{
    QString str = QString("%1: %2 (%3:%4, %5)\n")
                 .arg(prefix)
                 .arg(msg)
                 .arg(context.file)
                 .arg(context.line)
                 .arg(context.function);
    emit message(str);
    _messages.append(str);

    if (_messages.length() >= 50) {
        saveLogsToFile();
    }
}


void CustomLogger::saveLogsToFile()
{
    QString logFile = TCS::Utils::findUniqueLogFilename("log");
    QFile file(logFile);
    if (_messages.length() > 0 && file.open(QFile::Append)) {
        foreach (QString msg, _messages) {
            file.write(msg.toLocal8Bit());
        }
        file.close();
        _messages.clear();
    }
}


void CustomLogger::clearLog()
{
    saveLogsToFile();

    QString sLD = TCS::Utils::logDir();
    QDir ld (sLD);
    QStringList files = ld.entryList(QStringList() << "*.txt", QDir::Files);
    foreach (QString file, files) {
        QFile::remove(sLD + file);
    }
}
