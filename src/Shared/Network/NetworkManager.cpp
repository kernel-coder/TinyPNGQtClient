#include "NetworkManager.h"

/**
 * Constructor of NetworkManager class. Usually, user do not need to use
 * this class, instead use Ajax class.
 */
NetworkManager::NetworkManager(QObject *parent)
    : QNetworkAccessManager(parent), ShouldLogRequestResponse(true)
{

}

void NetworkManager::setLogRequestResponse(bool on)
{
    ShouldLogRequestResponse = on;
}

/**
 * Starts an url to load for HEAD operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress
 * @param url: url to open
 */
void NetworkManager::heaD(const QString &tag, const QUrl &url)
{
    initRequest(HeadOperation, tag, url);
}


/**
 * Starts an url to load for GET operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress
 * @param url: url to open
 */
void NetworkManager::geT(const QString &tag, const QUrl &url)
{
	initRequest(GetOperation, tag, url);
}


/**
 * Starts an url to load for POST operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress
 * @param url: url to open
 * @param data: data to post.
 */
void NetworkManager::posT(const QString &tag, const QUrl &url, const QByteArray &data)
{
    initRequest(PostOperation, tag, url, data);
}


/**
 * Starts an url to load for PUT operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress
 * @param url: url to open
 * @param data: data to put.
 */
void NetworkManager::puT(const QString &tag, const QUrl &url, const QByteArray &data)
{
    initRequest(PutOperation, tag, url, data);
}



void NetworkManager::posT(const QString &tag, const QUrl &url, const QMap<QString, QByteArray> &postParams,
                          const QString &fileName, const QString &fileParamName, const QString &fileContentType)
{
//    QString boundary = QString("---------------------------%1").arg(QDateTime::currentDateTime().currentMSecsSinceEpoch());
//    QString formBoundary = "\r\n--" + boundary + "\r\n";
//    QString formdataTemplate("Content-Disposition: form-data; name=\"%1\"\r\n\r\n");
//    QByteArray rawData;

//    QMapIterator<QString, QByteArray> iter(postParams);
//    while(iter.hasNext()) {
//        iter.next();
//        rawData.append(formBoundary);
//        rawData.append(formdataTemplate.arg(iter.key()));
//        rawData.append(iter.value());
//    }
//    rawData.append(formBoundary);

//    if (!fileName.isEmpty() && QFile::exists(fileName)) {
//        QString headerTemplate("Content-Disposition: form-data; name=\"%1\"; filename=\"%2\"\r\nContent-Type: %3\r\n\r\n");
//        rawData.append(headerTemplate.arg(fileParamName).arg(fileName.split('/').last()).arg(fileContentType));
//        rawData.append(QFile(fileName).readAll());
//    }

//    rawData.append(QString("\r\n--%1--\r\n").arg(boundary));
    QFile f(fileName); f.open(QFile::ReadOnly);
    QByteArray data = f.readAll();

    QNetworkRequest request(url); //our server with php-script
    request.setRawHeader("Content-Type",QString("application/octet-stream").toLatin1());
    request.setRawHeader("Content-Length", QString::number(data.length()).toLatin1());
	QByteArray auth = QByteArray("Basic ") + QByteArray("api:pjOaZ7vpi3jX-loMkErLvt_VIY2Phxv_").toBase64();
    request.setRawHeader("Authorization", auth);
    initRequest(PostOperation, tag, request, data);
}


/**
 * This method used internally by this class from geT, posT and puT method to
 * initialize corresponding requests.
 * @param opt: operation of type GET, POST and PUT
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param url: url to requests.
 */
void NetworkManager::initRequest(Operation opt, const QString &tag, const QUrl &url, const QByteArray &data)
{
    initRequest(opt, tag, QNetworkRequest(url), data);
}


void NetworkManager::initRequest(Operation opt, const QString &tag, const QNetworkRequest &request, const QByteArray &data)
{
    if (tag.isNull() || tag.isEmpty()) return;
    /*
     * If there is no requests currently for the given tag, initiates
     * the new one and bind to the QNetworkReplyObject's signals.
     */
    if (mCurrentRequests.contains(tag)){
        cancel(tag);
    }

    if (ShouldLogRequestResponse) {
        qDebug() << "Queueing Request: " << request.url().toString();
    }

    QNetworkReply *reply = 0;

    if (GetOperation == opt)
        reply = get(request);
    else if (HeadOperation == opt)
        reply = head(request);
    else if (PostOperation == opt)
        reply = post(request, data);
    else if (PutOperation == opt)
        reply = put(request, data);

    if (reply) {
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
        connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
        mCurrentRequests.insert(tag,reply);
    }
}


/**
 * This slot hitted when any request completed.
 */
void NetworkManager::onFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray location = reply->rawHeader("Location");
    QString tag = mCurrentRequests.key(reply,QString(""));
	QByteArray ddd = reply->readAll();
	if (!tag.isEmpty()) {
		/*
  * When downloading finished, let us emit it, unbound to signals and
  * finally schedule the allocated resources for deletion.
  */
		if (reply){
			int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			QNetworkReply::NetworkError err = reply->error();
			if (err != QNetworkReply::OperationCanceledError)
				emit requestProcesssed(tag, reply->readAll(), statusCode);

			disconnect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
			disconnect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
			disconnect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
			if (mCurrentRequests.contains(tag))
				mCurrentRequests.remove(tag);
			reply->deleteLater();
			reply = 0;
		}
	}
}


/**
 * This slot hitted indicating the progress of the underlying requests.
 */
void NetworkManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	/*
 * Progress of current request is in percentage.
 * Note: we are emitting the signal only when the total downloadable
 * size of the reply of current request is known.
 */
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	QString tag = mCurrentRequests.key(reply,QString(""));

	if (!tag.isEmpty()) {
		if (bytesTotal > 0){
			emit progress(tag, bytesReceived * 100 / bytesTotal);
		}
	}
}


/**
 * Users of this class can hit this slot to abort an ongoing request which
 * has tag id.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 */
void NetworkManager::cancel(const QString &tag)
{
	QNetworkReply* reply = mCurrentRequests.value(tag, 0);
	if (reply) {
		//reply->blockSignals(true);
		if (!reply->isFinished())
			reply->abort();
		mCurrentRequests.remove(tag);
		//reply->deleteLater();
	}
}


/**
 * This slot will be hitted if there is any erros in the ongoing requests.
 * This slot emmits the error signal for its user.
 * @param code : The error code.
 */
void NetworkManager::onError(QNetworkReply::NetworkError code)
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());      
	QString tag = mCurrentRequests.key(reply,QString(""));
	if (!tag.isEmpty()) {
		emit error(tag, int(code), reply->errorString());
	}
}
