#include "TableWidget.h"
#include <QNetworkAccessManager>
#include <QFile>
#include <QDebug>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObjectUserData>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QTableWidgetItem>
#include <QProgressBar>
#include <QLabel>
#include <QHeaderView>

#define FILE_TAG 1



struct TableWidgetPri {
    QNetworkAccessManager* NetMgr;
};

struct UserData : QObjectUserData {
    QString Filename;
    QLabel* LblStatus;
    QProgressBar* ProgressBar;

    UserData* clone()
    {
        UserData* ud = new UserData;
        ud->Filename = Filename;
        ud->LblStatus = LblStatus;
        ud->ProgressBar = ProgressBar;
        return ud;
    }
};


TableWidget::TableWidget(QWidget *p) : QTableWidget(0, 3, p), d(new TableWidgetPri)
{    
    QStringList cols; cols << "Status" << "Progress" << "File";
    setColumnWidth(0, 120);setColumnWidth(1, 160);
    setHorizontalHeaderLabels(cols);
    horizontalHeader()->setStretchLastSection(true);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
     setDropIndicatorShown(true);

     setTextElideMode(Qt::ElideMiddle);

    setDragDropMode(QAbstractItemView::DropOnly);
    d->NetMgr = new QNetworkAccessManager(this);
}

TableWidget::~TableWidget()
{
    delete d;
}


void TableWidget::startOptimizing()
{

}


void TableWidget::clearItems()
{

}


Qt::DropActions TableWidget::supportedDropActions() const {return Qt::LinkAction;}


void TableWidget::dragEnterEvent(QDragEnterEvent *e)
{
    const QMimeData* data = e->mimeData();
    foreach (QUrl url, data->urls()) {
        QString file = url.toString();
        if (file.endsWith(".png", Qt::CaseInsensitive) || file.endsWith(".jpg", Qt::CaseInsensitive)
                || file.endsWith(".jpeg", Qt::CaseInsensitive)) {
            e->acceptProposedAction();
            return;
        }
    }
    e->ignore();
}


void TableWidget::dragMoveEvent(QDragMoveEvent *e)
{
    e->accept();
}


void TableWidget::dropEvent(QDropEvent *e)
{
    const QMimeData* data = e->mimeData();
    foreach (QUrl url, data->urls()) {
        QString file = url.toString().replace("file:///", "");
        if (file.endsWith(".png", Qt::CaseInsensitive) || file.endsWith(".jpg", Qt::CaseInsensitive)
                || file.endsWith(".jpeg", Qt::CaseInsensitive)) {
            UserData* ud = new UserData;
            ud->Filename = file;
            ud->LblStatus = new QLabel("Starting...");
            ud->ProgressBar = new QProgressBar;ud->ProgressBar->setRange(0, 100);
            int row = rowCount(); insertRow(row);
            setCellWidget(row, 0, ud->LblStatus);setCellWidget(row, 1, ud->ProgressBar);
            setItem(row, 2, new QTableWidgetItem(ud->Filename));
            optimizeFile(ud);
        }
    }
}


void TableWidget::optimizeFile(UserData* ud)
{
    QFile fp(ud->Filename);
    if (fp.open(QFile::ReadOnly)) {
        ud->LblStatus->setText("Uploading...");
        QByteArray data = fp.readAll(); fp.close();
        QUrl url("https://api.tinify.com/shrink");
        QNetworkRequest req(url);
        req.setRawHeader("Content-Type",QString("application/octet-stream").toLatin1());
        req.setRawHeader("Content-Length", QString::number(data.length()).toLatin1());
        QByteArray auth = QByteArray("Basic ") + QByteArray("api:pjOaZ7vpi3jX-loMkErLvt_VIY2Phxv_").toBase64();
        req.setRawHeader("Authorization", auth);
        QNetworkReply* reply = d->NetMgr->post(req, data);
        reply->setUserData(FILE_TAG, (QObjectUserData*)ud);
        connect(reply, SIGNAL(uploadProgress(qint64,qint64)), SLOT(onUploadProgress(qint64,qint64)));
        connect(reply, SIGNAL(finished()), this, SLOT(onPostFinished()));        
    }
    else {
        ud->LblStatus->setText("Failed to open file");
    }
}


bool TableWidget::handleError(QNetworkReply *reply)
{
    bool noError = true;
    if (reply) {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QNetworkReply::NetworkError err = reply->error();
        noError = err == QNetworkReply::NoError;
        if (!noError) {
            qDebug() << QString("Error occured. Status code = %1, errcode = %2, msg = %3").arg(statusCode).arg(err).arg(reply->errorString());
            TinyPNGError tpErr;tpErr.importFromJson(reply->readAll());
            qDebug() << "TinyPNG error: " << tpErr.message();
        }
    }
    return noError;
}


void TableWidget::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        if (bytesTotal <= 0) return;
        UserData* ud = (UserData*)(reply->userData(FILE_TAG));
        ud->ProgressBar->setValue(bytesSent * 100 / bytesTotal);
        if (bytesSent == bytesTotal) {
            UserData* ud = (UserData*)(reply->userData(FILE_TAG));
            ud->LblStatus->setText("Optimizing...");
        }
    }
}


void TableWidget::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        if (bytesTotal <= 0) return;
        UserData* ud = (UserData*)(reply->userData(FILE_TAG));
        ud->ProgressBar->setValue(bytesReceived * 100 / bytesTotal);
    }
}


void TableWidget::onPostFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (handleError(reply)) {
        UserData* ud = (UserData*)(reply->userData(FILE_TAG));
        QByteArray location = reply->rawHeader("Location");
        QByteArray data = reply->readAll();
        TinyPNGPostOutput jsonOutput; jsonOutput.importFromJson(data);
        if (location.isEmpty()) {
            qDebug() << "Location header found empty for file " << ud->Filename;
        }
        else {
            ud->LblStatus->setText("Downloading...");
            QString filePath = location;
            QUrl url(filePath);
            QNetworkRequest req(url);
            QNetworkReply* reply2 = d->NetMgr->get(req);
            ud = ud->clone();
            reply2->setUserData(FILE_TAG, (QObjectUserData*)ud);
            connect(reply2, SIGNAL(downloadProgress(qint64,qint64)), SLOT(onDownloadProgress(qint64,qint64)));
            connect(reply2, SIGNAL(finished()), this, SLOT(onGetFinished()));
        }
        reply->deleteLater();
    }
}


void TableWidget::onGetFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (handleError(reply)) {
        UserData* ud = (UserData*)(reply->userData(FILE_TAG));
        ud->LblStatus->setText("Done!");
        QByteArray data = reply->readAll();
        QFile fp(ud->Filename);
        if (fp.open(QFile::WriteOnly)) {
            fp.write(data);fp.close();
        }
        reply->deleteLater();
    }
}
