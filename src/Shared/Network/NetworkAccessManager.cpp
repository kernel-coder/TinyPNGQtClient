#include "networkaccessmanager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QHash>

struct RequestInfo {
    RequestInfo(QNetworkAccessManager::Operation op, const QByteArray& data, QBuffer* buffer)
        : Operation(op), Data(data), Buffer(buffer){}
    QNetworkAccessManager::Operation Operation;
    QByteArray Data;
    QBuffer *Buffer;    
};

struct NetworkAccessManagerPri {
    NetworkAccessManagerPri() : ShouldLogRequestResponse(true) {}
    bool ShouldLogRequestResponse;
    QHash<QNetworkReply*, RequestInfo*> DataMap;
    bool removeRequestInfo(QNetworkReply* reply)
    {
        RequestInfo* ri = DataMap.value(reply, 0);
        if (ri) {
            DataMap.remove(reply);
            if (ri->Buffer) ri->Buffer->deleteLater();
            delete ri;
            return true;
        }
        return false;
    }
};

NetworkAccessManager* NetworkAccessManager::_me = 0;

NetworkAccessManager* NetworkAccessManager::Me()
{
    if (_me == 0) {
        _me = new NetworkAccessManager(QCoreApplication::instance());
    }
    return _me;
}


NetworkAccessManager::NetworkAccessManager(QObject *parent)
    : QNetworkAccessManager(parent), d(new NetworkAccessManagerPri)
{
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(onRequestFinished(QNetworkReply*)));
}


void NetworkAccessManager::setLogRequestResponse(bool on)
{
    d->ShouldLogRequestResponse = on;
}


QNetworkReply *	NetworkAccessManager::createRequest(Operation op, const QNetworkRequest & req, QIODevice * outgoingData)
{
    QByteArray data;
    QBuffer* buffer = 0;

    // cache the content first
    if (outgoingData) {
        buffer = new QBuffer(this);
        data = outgoingData->readAll();
        buffer->setData(data);
    }

    if (d->ShouldLogRequestResponse) {
        qDebug() << "Queueing Request: " << req.url().toString();
    }

    QNetworkReply* reply = QNetworkAccessManager::createRequest(op, req, buffer);
    reply->ignoreSslErrors();

    if (outgoingData) {
        qDebug() << "Request Details# op=" << op << ", url=" <<  req.url().toString() << ", data("<<data.length() <<")=" << data;
        if (d->removeRequestInfo(reply)) {
            qWarning() << "Same request aleady cached. So going to remove reference to old one.";
        }
        d->DataMap.insert(reply, new RequestInfo(op, data, buffer));
    }

    return reply;
}


void NetworkAccessManager::onRequestFinished(QNetworkReply *reply)
{
    // http://stackoverflow.com/questions/4330274/qtwebkit-how-to-check-http-status-code
    if(reply->error() > 0) {
        qDebug() << "Response Error " << reply->error() << "\n" << reply->errorString();
        if (reply->error() == QNetworkReply::ContentReSendError) {
            if (d->DataMap.contains(reply))  {
                RequestInfo* ri = d->DataMap.value(reply);
                qWarning() << "Hit with  205! Going to re-process the request for which this error is."
                           << " The request url is " << reply->request().url().toString()
                           << " and the request op-type is " << ri->Operation << ". FYI(head=1,get=2,put=3,post=4,del=5).";

                switch (ri->Operation) {
                case HeadOperation:
                    head(reply->request()); break;
                case GetOperation:
                    get(reply->request()); break;
                case PutOperation:
                    put(reply->request(), ri->Data); break;
                case PostOperation:
                    post(reply->request(), ri->Data); break;
                case DeleteOperation:
                    deleteResource(reply->request()); break;
                default:
                    qWarning() << "Alas! Unknown operation :("; break;
                }
            }
            else {
                qWarning()  << "Hit with  205! But no cached data found in the network-manager data map associated with this request to process the request!";
            }
        }
    }
    else {
        qDebug() << "Request processed successfully# " << reply->request().url().toString();
    }

    d->removeRequestInfo(reply);
}
