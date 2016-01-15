#include "ThreadedHttp.h"
#include "NetworkManager.h"

#include <QApplication>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMetaObject>

/**
 * This is the static method which returns the only one & ever exist instance
 * of type ThreadedHttp class.
 * @return The singletone instance of this class.
 */
ThreadedHttp* ThreadedHttp::me()
{
    static ThreadedHttp* me = 0;
    if (!me) {
        me = new ThreadedHttp;
        connect(qApp, SIGNAL(aboutToQuit()), me, SLOT(deleteLater()));
        me->synchornousStart();
    }
    return me;
}


void ThreadedHttp::synchornousStart()
{
    mMutex.lock();
    qDebug() << "ThreadedHttp: WAITING_TO_START";
    start();
    mWaitCondition.wait(&mMutex);
    mMutex.unlock();
    qDebug() << "ThreadedHttp: STARED";
}


NetworkManager* ThreadedHttp::netManager()const
{
    return mNetManager;
}

/**
 * Consturctor of this class.
 * Note: Constructor is made protected not to allow user to create instance
 * of this class, instead use the me() static method.
 */
ThreadedHttp::ThreadedHttp(QObject *parent) :
    QThread(parent), mNetManager(0)
{
}


/**
 * The runnable method of thread class, i.e: run method runs on a user thread.
 */
void ThreadedHttp::run()
{      
    mNetManager = new NetworkManager;

    connect(this, SIGNAL(geT(QString,QUrl)), mNetManager, SLOT(geT(QString,QUrl)));
    connect(this, SIGNAL(heaD(QString,QUrl)), mNetManager, SLOT(heaD(QString,QUrl)));
    connect(this, SIGNAL(posT(QString,QUrl,QByteArray)), mNetManager, SLOT(posT(QString,QUrl,QByteArray)));
    connect(this, SIGNAL(posT(QString,QUrl,QMap<QString,QByteArray>,QString,QString,QString)),  mNetManager,
            SLOT(posT(QString,QUrl,QMap<QString,QByteArray>,QString,QString,QString)));
    connect(this, SIGNAL(puT(QString,QUrl,QByteArray)), mNetManager, SLOT(puT(QString,QUrl,QByteArray)));
    connect(this, SIGNAL(canceL(QString)), mNetManager, SLOT(cancel(QString)));

    connect(mNetManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(onAuthenticationRequired(QNetworkReply*,QAuthenticator*)));
    connect(mNetManager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            this, SLOT(onProxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
    connect(mNetManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            this, SLOT(onSslErrors(QNetworkReply*,QList<QSslError>)));

    connect(mNetManager, SIGNAL(requestProcesssed(QString,QByteArray,int)), this, SLOT(onFinished(QString,QByteArray,int)));
    connect(mNetManager, SIGNAL(progress(QString,int)), this, SIGNAL(progress(QString,int)));
    connect(mNetManager, SIGNAL(error(QString,int, QString)), this, SIGNAL(error(QString,int, QString)));

    msleep(500);
    mWaitCondition.wakeAll();
    exec();
}


/**
 * Invokes the given method on instance of NetworkManager with the given parameters.
 * @param method: method to be invoked on NetworkManager instance.
 * @param tag: First argument to pass to invokable method.
 * @param url: Second argument to pass to invokable method.
 * @param useData: Indicates whether thirgh param, data, should be send to invokable method.
 * @param data: Third argument to pass to invokable method.
 */
void ThreadedHttp::invokeRequest(const char *method, const QString &tag,
                                 const QUrl &url, bool useData, const QByteArray &data)
{
    if (useData)
        QMetaObject::invokeMethod(mNetManager, method, Qt::AutoConnection,
                                  Q_ARG(QString, tag), Q_ARG(QUrl, url), Q_ARG(QByteArray, data));
    else
        QMetaObject::invokeMethod(mNetManager, method, Qt::AutoConnection,
                                  Q_ARG(QString, tag), Q_ARG(QUrl, url));
}


/**
 * Starts an url to load for HEAD operation.
 * @param tag: an id to this request and this id will be used to notify
 *             when data loaded, error occured and progress progress.
 * @param url: path to open
 */
void ThreadedHttp::head(const QString &tag, const QUrl &url)
{
    emit heaD(tag, url);
}


/**
 * Starts an url to load for GET operation.
 * @param tag: an id to this request and this id will be used to notify
 *             when data loaded, error occured and progress progress.
 * @param url: path to open
 */
void ThreadedHttp::get(const QString &tag, const QUrl &url)
{
    emit geT(tag, url);
}


/**
 * Starts an url to load for POST operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress
 * @param url: url to open
 * @param data: data to post.
 */
void ThreadedHttp::post(const QString &tag, const QUrl &url, const QByteArray &data)
{
    emit posT(tag, url, data);
}


void ThreadedHttp::post(const QString &tag, const QUrl &url, const QMap<QString, QByteArray> &postParams, const QString &fileName, const QString &fileParamName, const QString &fileContentType)
{
    emit posT(tag, url, postParams, fileName, fileParamName, fileContentType);
}


/**
 * Starts an url to load for PUT operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress
 * @param url: url to open
 * @param data: data to put.
 */
void ThreadedHttp::put(const QString &tag, const QUrl &url, const QByteArray &data)
{
    emit puT(tag, url, data);
}


/**
 * This private slot hitted when any request completed. It will also emmits finished
 * signal to the user of this class to indicate that the tagged requests finished.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param data: data returned by the host.
 * @param httpStatusCode: the http status code for this request.
 */
void ThreadedHttp::onFinished(const QString &tag, const QByteArray &data, int httpStatusCode)
{
    emit finished(tag, data, httpStatusCode);

    if (NetworkManager::Unauthorized == httpStatusCode) {
        emit needToPerformAuthorization();
    }
}


/**
 * Users of this class can hit this slot to abort an ongoing request which
 * has tag id.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 */
void ThreadedHttp::cancel(const QString &tag)
{
    if (mNetManager)
        emit canceL(tag);
    /* QMetaObject::invokeMethod(mNetManager, "cancel",
                                Qt::AutoConnection,
                                Q_ARG(QString, tag));
             */
}



/**
 * Utils type method to show an input dialog for username/password for
 * the authenticationRequired and proxyAuthenticationRequired signals
 * from NetworkManager
 */
static void getUserPassFromUser(const QString &title, const QString &host, QAuthenticator *authenticator)
{
    QLineEdit *leUser = new QLineEdit;
    QLineEdit *lePass = new QLineEdit;
    lePass->setEchoMode(QLineEdit::Password);
    QFormLayout *form = new QFormLayout;
    form->addRow(QObject::tr("Username:"), leUser);
    form->addRow(QObject::tr("Password:"),lePass);
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(new QLabel(QObject::tr("%1 at %2").arg(authenticator->realm()).arg(host)));
    vbox->addLayout(form);
    QDialog *dlg = new QDialog;
    dlg->setWindowTitle(title);
    dlg->setLayout(vbox);

    if (dlg->exec() == QDialog::Accepted) {
        authenticator->setUser(leUser->text());
        authenticator->setPassword(lePass->text());
    }
    dlg->deleteLater();
}


/**
 * This slot will be hitted when authenticaiton is required and not provided.
 */
void ThreadedHttp::onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    getUserPassFromUser(tr("Authentication Required"), reply->request().url().host(), authenticator);
}


/**
 * This slot will be hitted when proxy authenticaiton is required and not provided.
 */
void ThreadedHttp::onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    getUserPassFromUser(tr("Proxy Authentication Required"), proxy.hostName(), authenticator);
}


/**
 * This slot will be hitted when there is any ssl errors. Show messages to user while there are ssl erros.
 */
void ThreadedHttp::onSslErrors(QNetworkReply*reply, const QList<QSslError> &errors)
{
    reply->ignoreSslErrors();
    /*QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += ", ";
        errorString += error.errorString();
    }

    if (QMessageBox::warning(0, tr("HTTP"),
                             tr("One or more SSL errors has occurred: %1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }*/
}
