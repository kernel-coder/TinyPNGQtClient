#ifndef CUSTOMLOGGER_H
#define CUSTOMLOGGER_H

#include <QThread>
#include <QFile>
#include <QMessageLogContext>
#include <QStringList>

class QTimer;

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
void appCleanup();

class CustomLogger : public QObject
{
    Q_OBJECT
    explicit CustomLogger(QObject* p = 0);
public:    
    ~CustomLogger();
    static CustomLogger* me();
    void log(const QString& prefix, const QMessageLogContext &context, const QString &msg);

signals:
    void message(const QString& msg);

public slots:
    void clearLog();

private:
    void saveLogsToFile();

private:
    QStringList _messages;
};

#endif // CUSTOMLOGGER_H
