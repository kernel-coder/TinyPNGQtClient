#ifndef AJAX_H
#define AJAX_H

#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include <QMap>
#include <QStringList>

typedef QMap<QString, QByteArray> StringMap;
typedef QMapIterator<QString, QByteArray> StringMapIterator;

/**
 * This class get/post/put data from a given url. When data arrived,
 * it emits the finished signal. 
 */
class Ajax : public QObject
{
	Q_OBJECT
public:
	Ajax(QObject *parent = 0);
	~Ajax();

    QUrl lastRequest(const QString& tag) const;

	/*
  * Returns the last error code for the given tag.
  */
    QNetworkReply::NetworkError lastErrorCode(const QString &tag) const;

    /*
         * Returns the last error message for the given tag.
         */
    QString lastErrorMsg(const QString &tag) const;

	/*
  * Returns the http code for the given tag.
  */
	int httpStatusCode(const QString &tag) const;


signals:
    /*
   * This signal emits when data loaded completey.
   */
    void finished(const QString &tag, const QByteArray& data);

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
        * Starts an url to load with HEAD operation.
        */
    void head(const QString &tag, const QString& url);
    void head(const QString &tag, const QUrl &url);

	/*
  * Starts an url to load with GET operation.
  */
	void get(const QString &tag, const QString& host, const QString &path,
			 int port, const QString &user = QString(),
			 const QString &pass = QString(), const StringMap &paramsMap = StringMap());
	void get(const QString &tag, const QString& url, const QString &user = QString(),
			 const QString &pass = QString(), const StringMap &paramsMap = StringMap());
	void get(const QString &tag, const QUrl &url);

	/*
  * Starts an url to load with POST operation.
  */
	void post(const QString &tag, const QString& host, const QString &path, const QByteArray &data,
			  int port = 80, const QString &user = QString(),
			  const QString &pass = QString(), const StringMap &paramsMap = StringMap());
	void post(const QString &tag, const QString& url,  const QByteArray &data,
			  const QString &user = QString(), const QString &pass = QString(), const StringMap &paramsMap = StringMap());
	void post(const QString &tag, const QUrl &url, const QByteArray &data);
    void post(const QString& tag, const QString &url, const StringMap& postParams,
              const QString& fileName = QString(), const QString& fileParamName = QString(), const QString& fileContentType = QString(),
              const QString &user = QString(), const QString &pass = QString());

	/*
  * Starts an url to load with PUT operation.
  */
	void put(const QString &tag, const QString& host, const QString &path, const QByteArray &data,
			 int port = 80, const QString &user = QString(),
			 const QString &pass = QString(), const StringMap &paramsMap = StringMap());
    void put(const QString &tag, const QString& url,  const QByteArray &data, const QString &user = QString(),
			 const QString &pass = QString(), const StringMap &paramsMap = StringMap());
	void put(const QString &tag, const QUrl &url, const QByteArray &data);

	/*
  * Cancel the current request if thre is any.
  */
	void cancel(const QString &tag);


private slots:
    /*
         * This private slot hitted when any request completed.
         */
    void onFinished(const QString &tag, const QByteArray& data, int httpStatusCode);

    /*
         * This private slots hitted when there is a progress of the tagged requests.
         */
    void onProgress(const QString &tag, int percent);

    /*
         * This private slots hitted when there is an error of the tagged requests.
         */
    void onError(const QString &tag, int code, const QString &errStr);

private:
    void clearMapInfo(const QString &tag);

private:
    Q_DISABLE_COPY(Ajax)

    QMap<QString, int> mErrorCodes;
    QMap<QString, QString> mErrorMsgs;
    QMap<QString, int> mHttpStatusCodes;
    QMap<QString, QUrl> mRequests;
};

#endif // AJAX_H
