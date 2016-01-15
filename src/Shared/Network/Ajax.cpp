#include "Ajax.h"
#include "ThreadedHttp.h"
#include "NetworkManager.h"

/**
 * Constructor of Ajax class
 */
Ajax::Ajax(QObject *parent)
    : QObject(parent)
{
	ThreadedHttp *http = ThreadedHttp::me();
	connect(http, SIGNAL(finished(QString,QByteArray, int)), this, SLOT(onFinished(QString,QByteArray, int)));
	connect(http, SIGNAL(progress(QString,int)), this, SLOT(onProgress(QString,int)));
	connect(http, SIGNAL(error(QString,int, QString)), this, SLOT(onError(QString,int, QString)));
}


/**
 * Destructor of Ajax class
 */
Ajax::~Ajax()
{
	ThreadedHttp *http = ThreadedHttp::me();
	disconnect(http, SIGNAL(finished(QString,QByteArray, int)), this, SLOT(onFinished(QString,QByteArray, int)));
	disconnect(http, SIGNAL(progress(QString,int)), this, SLOT(onProgress(QString,int)));
	disconnect(http, SIGNAL(error(QString,int, QString)), this, SLOT(onError(QString,int, QString)));
}


/**
 * Starts an url to load for HEAD operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param url: url to hit.
 */
void Ajax::head(const QString &tag, const QString &url)
{
    head(tag, QUrl(url));
}


/**
 * Starts an url to load for HEAD operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param url: url to hit.
 */
void Ajax::head(const QString &tag, const QUrl &url)
{
    clearMapInfo(tag);
    mRequests.insert(tag, url);
    ThreadedHttp::me()->head(tag, url);
}


/**
 * Starts an url to load for GET operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param host: host name of the url to load (like google.com)
 * @param path: path inside the host (search)
 * @param port: port of the host to open
 * @param user: username if any authentication requires.
 * @param pass: password if any authentication requires.
 * @param paramsMap: a map of the parameter to add for this requests.
 */
void Ajax::get(const QString &tag, const QString &host, const QString &path,
               int port, const QString &user, const QString &pass, const StringMap &paramsMap)
{
	QUrl url;
	if (!host.startsWith("http://"))
		url.setScheme("http");
	url.setHost(host);
	url.setPath(path);
	url.setPort(port);
	url.setUserName(user);
	url.setPassword(pass);
	StringMapIterator iter(paramsMap);
    QUrlQuery query;
	while (iter.hasNext())
        query.addQueryItem(iter.key(), iter.value());
    url.setQuery(query);
	get(tag, url);
}


/**
 * Starts an url to load for GET operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param url: url to hit.
 * @param user: username if any authentication requires.
 * @param pass: password if any authentication requires.
 * @param paramsMap: a map of the parameter to add for this requests.
 */
void Ajax::get(const QString &tag, const QString &url, const QString &user,
               const QString &pass, const StringMap &paramsMap)
{      
    QUrl link(url);
    link.setUserName(user);
    link.setPassword(pass);
    StringMapIterator iter(paramsMap);
    QUrlQuery query;
    while (iter.hasNext()) {
        query.addQueryItem(iter.key(), iter.value());
    }
    link.setQuery(query);
    get(tag, link);
}


/**
 * Starts an url to load for GET operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param url: url to hit.
 */
void Ajax::get(const QString &tag, const QUrl &url)
{
	clearMapInfo(tag);
    mRequests.insert(tag, url);
	ThreadedHttp::me()->get(tag, url);
}


/**
 * Starts an url to load for POST operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param host: host name of the url to load (like google.com)
 * @param path: path inside the host (search)
 * @param data: data to post
 * @param port: port of the host to open
 * @param user: username if any authentication requires.
 * @param pass: password if any authentication requires.
 * @param paramsMap: a map of the parameter to add for this requests.
 */
void Ajax::post(const QString &tag, const QString &host, const QString &path,
                const QByteArray &data, int port, const QString &user,
                const QString &pass, const StringMap &paramsMap)
{
    QUrl url;
    if (!host.startsWith("http://"))
        url.setScheme("http");
    url.setHost(host);
    url.setPath(path);
    url.setPort(port);
    url.setUserName(user);
    url.setPassword(pass);
    StringMapIterator iter(paramsMap);
    QUrlQuery query;
    while (iter.hasNext())
        query.addQueryItem(iter.key(), iter.value());
    url.setQuery(query);
    post(tag, url, data);
}


/**
 * Starts an url to load for POST operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param url: url to hit
 * @param data: data to post
 * @param user: username if any authentication requires.
 * @param pass: password if any authentication requires.
 * @param paramsMap: a map of the parameter to add for this requests.
 */
void Ajax::post(const QString &tag, const QString &url, const QByteArray &data,
                const QString &user, const QString &pass, const StringMap &paramsMap)
{
    QUrl link(url);
    link.setUserName(user);
    link.setPassword(pass);
    StringMapIterator iter(paramsMap);
    QUrlQuery query;
    while (iter.hasNext()) {
        query.addQueryItem(iter.key(), iter.value());
    }
    link.setQuery(query);
    post(tag, link, data);
}


/**
 * Starts an url to load for POST operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param url: url to hit.
 * @param data: data to post
 */
void Ajax::post(const QString &tag, const QUrl &url, const QByteArray &data)
{
    clearMapInfo(tag);
    mRequests.insert(tag, url);
    ThreadedHttp::me()->post(tag, url,data);
}

void Ajax::post(const QString &tag, const QString &url, const StringMap &postParams,
                const QString &fileName, const QString &fileParamName, const QString &fileContentType, const QString &user, const QString &pass)
{
    QUrl link(url);
    link.setUserName(user);
    link.setPassword(pass);
    clearMapInfo(tag);
    mRequests.insert(tag, link);
    ThreadedHttp::me()->post(tag, link, postParams, fileName, fileParamName, fileContentType);
}


/**
 * Starts an url to load for PUT operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param host: host name of the url to load (like google.com)
 * @param path: path inside the host (search)
 * @param data: data to put
 * @param port: port of the host to open
 * @param user: username if any authentication requires.
 * @param pass: password if any authentication requires.
 * @param paramsMap: a map of the parameter to add for this requests.
 */
void Ajax::put(const QString &tag, const QString &host, const QString &path,
               const QByteArray &data, int port, const QString &user,
               const QString &pass, const StringMap &paramsMap)
{
    QUrl url;
    if (!host.startsWith("http://"))
        url.setScheme("http");
    url.setHost(host);
    url.setPath(path);
    url.setPort(port);
    url.setUserName(user);
    url.setPassword(pass);
    StringMapIterator iter(paramsMap);
    QUrlQuery query;
    while (iter.hasNext())
        query.addQueryItem(iter.key(), iter.value());
    url.setQuery(query);
    put(tag, url, data);
}


/**
 * Starts an url to load for PUT operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param url: url to hit
 * @param data: data to put
 * @param user: username if any authentication requires.
 * @param pass: password if any authentication requires.
 * @param paramsMap: a map of the parameter to add for this requests.
 */
void Ajax::put(const QString &tag, const QString &url, const QByteArray &data,
               const QString &user, const QString &pass, const StringMap &paramsMap)
{
    QUrl link(url);
    link.setUserName(user);
    link.setPassword(pass);
    StringMapIterator iter(paramsMap);
    QUrlQuery query;
    while (iter.hasNext()) {
        query.addQueryItem(iter.key(), iter.value());
    }
    link.setQuery(query);
    put(tag, link, data);
}


/**
 * Starts an url to load for PUT operation.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param url: url to hit.
 * @param data: data to put
 */
void Ajax::put(const QString &tag, const QUrl &url, const QByteArray &data)
{
    clearMapInfo(tag);
    mRequests.insert(tag, url);
    ThreadedHttp::me()->put(tag, url,data);
}


/**
 * Users of this class can hit this slot to abort an ongoing request which
 * has tag id.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 */
void Ajax::cancel(const QString &tag)
{
    ThreadedHttp::me()->cancel(tag);
}


/**
 * This private slot hitted when any request completed. It will also emmits finished
 * signal to the user of this class to indicate that the tagged requests finished.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param data: data returned by the host.
 * @param httpStatusCode: the http status code for this request.
 */
void Ajax::onFinished(const QString &tag, const QByteArray &data, int httpStatusCode)
{
	if (mRequests.contains(tag)) {
		mHttpStatusCodes.insert(tag, httpStatusCode);
		emit finished(tag, data);	
	}
}


/**
 * This private slots hitted when there is a progress of the tagged requests.
 * It will emmits progress signal to the user of this class to indicate the
 * progress of the tagged requests.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param percent: percent completed.
 */
void Ajax::onProgress(const QString &tag, int percent)
{
    if (mRequests.contains(tag)) {
		emit progress(tag, percent);
	}
}


/**
 * This private slots hitted when there is an error of the tagged requests.
 * It will emmits error signal to the user of this class to indicate the
 * error of the tagged requests.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @param code: error code
 * @param errStr: error message
 */
void Ajax::onError(const QString &tag, int code, const QString &errStr)
{
	if (mRequests.contains(tag)) {
		mErrorCodes.insert(tag, code);
		mErrorMsgs.insert(tag, errStr);
		emit error(tag, code, errStr);
	}
}


QUrl Ajax::lastRequest(const QString &tag) const
{
    return mRequests.value(tag, QUrl());
}


/**
 * If any user of this class does not bind the the error signal, it can
 * use this read-only property to get the error occured for the given
 * taggeg request.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @return error code of type QNetworkReply::NetworkError if any occur
 * for the given tagged requests.
 */
QNetworkReply::NetworkError Ajax::lastErrorCode(const QString &tag) const
{
    if (mErrorCodes.contains(tag))
        return QNetworkReply::NetworkError(mErrorCodes.value(tag));
    else
        return QNetworkReply::NoError;
}


/**
 * If any user of this class does not bind the the error signal, it can
 * use this read-only property to get the error occured for the given
 * taggeg request.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 * @return error message if any occur for the given tagged requests.
 */
QString Ajax::lastErrorMsg(const QString &tag) const
{
    return mErrorMsgs.value(tag, "");
}


/**
 * Returns the http code for the given tag.
 * @param tag: an id to this request and this id will be used to notify
 *              when data loaded, error occured and progress progress.
 */
int Ajax::httpStatusCode(const QString &tag) const
{
    return mHttpStatusCodes.value(tag,0);
}


void Ajax::clearMapInfo(const QString &tag)
{
    if (mRequests.contains(tag)) {
        mRequests.remove(tag);
    }
    if(mHttpStatusCodes.contains(tag))
        mHttpStatusCodes.remove(tag);
    if(mErrorCodes.contains(tag))
        mErrorCodes.remove(tag);
    if(mErrorMsgs.contains(tag))
        mErrorMsgs.remove(tag);
}
