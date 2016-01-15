#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QtNetwork>


/**
 * This class replacement of QNetworkAccessManager.
 * QNetworkAccessManager does not support loading url vai slots.
 * It is designed to communicate with an id for each request. It uses that
 * id to emmit result, error and progress for that particular requests.
 * NOTE: Tag means an unique id.
 * NOTE: If there is a request that is not finished yet of same tag, requesting
 * for a new url, it will abort the previous one.
 */
class NetworkManager : public QNetworkAccessManager
{
	Q_OBJECT
public:
	enum HttpStatusCodes
	{
		OK = 200,
		Unauthorized = 401
	};
	explicit NetworkManager(QObject *parent = 0);
    void setLogRequestResponse(bool on);

signals:
    /*
         * This signal emits when data loaded completey.
         */
    void requestProcesssed(const QString &tag, const QByteArray& data, int httpStatusCode);

	/*
  * Progress of current request is in percentage.
  */
	void progress(const QString &tag, int percent);

	/*
  * Signals an error if there is any.
  */
	void error(const QString &tag, int code, const QString &errStr);


public slots:
    /*
         * Starts an url to load for GET operation.
         */
    void heaD(const QString &tag, const QUrl &url);

	/*
  * Starts an url to load for GET operation.
  */
	void geT(const QString &tag, const QUrl &url);

	/*
  * Starts an url to load for POST operation.
  */
	void posT(const QString &tag, const QUrl &url, const QByteArray &data);

	/*
  * Starts an url to load for PUT operation.
  */
	void puT(const QString &tag, const QUrl &url, const QByteArray &data);

    void posT(const QString& tag, const QUrl &url, const QMap<QString, QByteArray>& postParams,
              const QString& fileName, const QString& fileParamName, const QString& fileContentType);

	/*
  * Cancel the current request with this tag if thre is any.
  */
	void cancel(const QString &tag);


private slots:
    /*
         * This private slot hitted when any request completed.
         */
	void onFinished();

    /*
         * This private slots hitted when there is a progress of the tagged requests.
         */
	void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    /*
         * This private slots hitted when there is an error of the tagged requests.
         */
	void onError(QNetworkReply::NetworkError code);


private:
	/*
  * Internally used to initiate a request.
  */
	void initRequest(Operation opt, const QString &tag, const QUrl &url, const QByteArray &data = QByteArray());
    void initRequest(Operation opt, const QString& tag, const QNetworkRequest& request, const QByteArray &data = QByteArray());


private:
	Q_DISABLE_COPY(NetworkManager)
    bool ShouldLogRequestResponse;
	QMap<QString, QNetworkReply*> mCurrentRequests;
};

#endif // NETWORKMANAGER_H
