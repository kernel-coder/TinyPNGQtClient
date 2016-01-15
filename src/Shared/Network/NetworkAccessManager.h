#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QtNetwork>

struct NetworkAccessManagerPri;


class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
    explicit NetworkAccessManager(QObject *parent = 0);
public:
    static NetworkAccessManager* Me();
    void setLogRequestResponse(bool on);

protected:
    QNetworkReply *	createRequest(Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0);

private slots:
    void onRequestFinished(QNetworkReply *reply);

private:
    Q_DISABLE_COPY(NetworkAccessManager)
    static NetworkAccessManager* _me;
    NetworkAccessManagerPri *d;
};

#endif // NETWORKACCESSMANAGER_H
