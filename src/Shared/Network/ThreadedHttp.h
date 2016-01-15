#ifndef THREADEDHTTP_H
#define THREADEDHTTP_H

#include <QThread>
#include <QtNetwork>
#include <QMutex>
#include <QWaitCondition>

class NetworkManager;

/**
 * This class is designed to run QNetworkAccessManager under an user thread.
 * This is a singleton class. Applicaiton should use this class insted of
 * directly using QNetworkAccessManager.
 */
class ThreadedHttp : public QThread
{
    Q_OBJECT
protected:
    explicit ThreadedHttp(QObject *parent = 0);


public:
    static ThreadedHttp* me();
    NetworkManager* netManager() const;

signals:
    /*
       * Starts an url to load for HEAD operation.
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

    void posT(const QString& tag, const QUrl &url, const QMap<QString, QByteArray>& postParams,
              const QString& fileName, const QString& fileParamName, const QString& fileContentType);

    /*
       * Starts an url to load for PUT operation.
       */
    void puT(const QString &tag, const QUrl &url, const QByteArray &data);
    /*
       * Cancel the current request with this tag if thre is any.
       */
    void canceL(const QString &tag);


    /*
         * This signal emits when data loaded completey.
         */
    void finished(const QString &tag, const QByteArray& data, int httpStatusCode);

    /*
         * Progress of current request is in percentage.
         */
    void progress(const QString &tag, int percent);

    /*
         * Signals an error if there is any.
         */
    void error(const QString &tag, int code, const QString &errStr);

    /*
         * When status codes with 401 comes against a request, this signal emitted.
         */
    void needToPerformAuthorization();


public slots:
    /*
         * Starts an url to load for HEAD operation.
         */
    void head(const QString &tag, const QUrl &url);

	/*
  * Starts an url to load for GET operation.
  */
	void get(const QString &tag, const QUrl &url);

	/*
  * Starts an url to load for POST operation.
  */
	void post(const QString &tag, const QUrl &url, const QByteArray &data);
    void post(const QString& tag, const QUrl &url, const QMap<QString, QByteArray>& postParams,
              const QString& fileName, const QString& fileParamName, const QString& fileContentType);

	/*
  * Starts an url to load for PUT operation.
  */
	void put(const QString &tag, const QUrl &url, const QByteArray &data);

	/*
  * Cancel the current request if thre is any.
  */
	void cancel(const QString &tag);


protected:
    /*
         * The thread-method, i.e: this method will run on background thread.
         */
    virtual void run();


private slots:
    void onFinished(const QString &tag, const QByteArray& data, int httpStatusCode);

    /*
         * This slot will be hitted when authenticaiton is required and not provided.
         */
    void onAuthenticationRequired ( QNetworkReply * reply,
                                    QAuthenticator * authenticator);

    /*
         * This slot will be hitted when proxy authenticaiton is required and not provided.
         */
    void onProxyAuthenticationRequired ( const QNetworkProxy & proxy,
                                         QAuthenticator * authenticator);

    /*
         * This slot will be hitted when there is any ssl errors.
         */
    void onSslErrors(QNetworkReply*,const QList<QSslError> &errors);


private:
    void synchornousStart();
    void invokeRequest(const char *method, const QString &tag, const QUrl &url,
                       bool useData = false, const QByteArray &data = QByteArray());


private:
    Q_DISABLE_COPY(ThreadedHttp)
    NetworkManager *mNetManager;
    QMutex mMutex;
    QWaitCondition mWaitCondition;
};

#endif // THREADEDHTTP_H
