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
#include "AppSettingsManager.h"
#include <QDir>
#include <QMessageBox>

#define FILE_TAG 1

#define COL_STATUS 0
#define COL_PROGRESS 1
#define COL_COMPRESSION_INFO 2
#define COL_FILE_NAME 3

struct MetaData : public QObjectUserData {
    enum State {None, Started, Uploading, Optimizing, Downloading, Done, Error};
    MetaData () : Root(0), CurrentState(None){}
    QString Filename;
    QLabel* LblStatus;
    QProgressBar* ProgressBar;
    QLabel* LblCompressionInfo;

    MetaData* clone()
    {
        MetaData* ud = new MetaData;
        ud->CurrentState = CurrentState;
        ud->Filename = Filename;
        ud->LblStatus = LblStatus;
        ud->ProgressBar = ProgressBar;
        ud->LblCompressionInfo = LblCompressionInfo;
        ud->Root = this->Root ? this->Root : this;
        return ud;
    }

    MetaData* Root;

    void setState(State state) {CurrentState = state; if (this->Root) this->Root->CurrentState = state;}
    State getState() const {return CurrentState;}

private:
    State CurrentState;
};
Q_DECLARE_METATYPE(MetaData)
Q_DECLARE_OPAQUE_POINTER(MetaData)


struct TableWidgetPri {
    QNetworkAccessManager* NetMgr;

    QString formatSize(qint64 size)
    {
        QString unit = "B";
        double s = size;
        if (s >= 1024) {
            s /= 1024;
            unit = "kB";
        }
        if (s >= 1024) {
            s /= 1024;
            unit = "MB";
        }
        if (s >= 1024) {
            s /= 1024;
            unit = "GB";
        }
        return QString("%1 %2").arg(QString::number(s, 'f', 2)).arg(unit);
    }
};


TableWidget::TableWidget(QWidget *p) : QTableWidget(0, 4, p), d(new TableWidgetPri)
{    
    QStringList cols; cols << "Status" << "Progress" <<""  <<"Filename";
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
    for (int i = rowCount() - 1; i >= 0; i--) {
        QTableWidgetItem* twi = item(i, COL_FILE_NAME);
        MetaData* ud = (MetaData*)(twi->data(Qt::UserRole).value<void*>());
        if (ud->getState() == MetaData::None || ud->getState() == MetaData::Error) {
            optimizeFile(ud->clone());
        }
    }
}


void TableWidget::clearItems()
{
    for (int i = rowCount() - 1; i >= 0; i--) {
        QTableWidgetItem* twi = item(i, COL_FILE_NAME);
        MetaData* ud = (MetaData*)(twi->data(Qt::UserRole).value<void*>());
        if (ud->getState() == MetaData::Done || (ud->getState() == MetaData::Error && ADSM->data()->removeErrorItems())) {
            removeRow(i);
            delete ud;
        }
    }
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
        QString file;
        if(url.isLocalFile()) file = url.toLocalFile();
        else file = url.toString().replace("file:///", "");
        if (file.endsWith(".png", Qt::CaseInsensitive) || file.endsWith(".jpg", Qt::CaseInsensitive)
                || file.endsWith(".jpeg", Qt::CaseInsensitive)) {
            MetaData* ud = new MetaData;
            ud->Filename = file;
            ud->LblStatus = new QLabel("...");
            ud->ProgressBar = new QProgressBar;ud->ProgressBar->setRange(0, 100);
            ud->LblCompressionInfo = new QLabel(d->formatSize(QFileInfo(file).size()) + "/?/?");
            int row = rowCount(); insertRow(row);
            setCellWidget(row, COL_STATUS, ud->LblStatus);setCellWidget(row, COL_PROGRESS, ud->ProgressBar);
            setCellWidget(row, COL_COMPRESSION_INFO, ud->LblCompressionInfo);
            QTableWidgetItem* twi = new QTableWidgetItem(ud->Filename); setItem(row, COL_FILE_NAME, twi);
            QVariant v = qVariantFromValue((void *) ud);
            twi->setData(Qt::UserRole, v);

            if (ADSM->data()->startsOptimizingWhenDrop()) {
                optimizeFile(ud->clone());
            }
        }
    }
}


void TableWidget::optimizeFile(MetaData* ud)
{
    if (ADSM->data()->isValid() == false) {
        QMessageBox::warning(this, "Wrong API Info", "Please provide correct API url and API key from the Settings window and they try again.");
    }
    QFile fp(ud->Filename);
    if (fp.open(QFile::ReadOnly)) {
        ud->LblStatus->setText("Uploading...");
        ud->setState(MetaData::Uploading);
        QByteArray data = fp.readAll(); fp.close();
        QUrl url(ADSM->data()->apiUrl());
        QNetworkRequest req(url);
        req.setRawHeader("Content-Type",QString("application/octet-stream").toLatin1());
        req.setRawHeader("Content-Length", QString::number(data.length()).toLatin1());
        QByteArray auth = QByteArray("Basic ") + QString("api:%1").arg(ADSM->data()->apiKey()).toLatin1().toBase64();
        req.setRawHeader("Authorization", auth);
        QNetworkReply* reply = d->NetMgr->post(req, data);
        reply->setUserData(FILE_TAG, (QObjectUserData*)ud);
        connect(reply, SIGNAL(uploadProgress(qint64,qint64)), SLOT(onUploadProgress(qint64,qint64)));
        connect(reply, SIGNAL(finished()), this, SLOT(onPostFinished()));        
    }
    else {
        ud->setState(MetaData::Error);
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
            MetaData* ud = (MetaData*)(reply->userData(FILE_TAG));
            ud->setState(MetaData::Error);
            qDebug() << QString("Error occured. Status code = %1, errcode = %2, msg = %3").arg(statusCode).arg(err).arg(reply->errorString());
            TinyPNGError tpErr;tpErr.importFromJson(reply->readAll());
            qDebug() << "TinyPNG error: " << tpErr.message();
            ud->LblStatus->setText("Error: " + tpErr.message());
        }
    }
    return noError;
}


void TableWidget::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        if (bytesTotal <= 0) return;
        MetaData* ud = (MetaData*)(reply->userData(FILE_TAG));
        ud->ProgressBar->setValue(bytesSent * 100 / bytesTotal);
        if (bytesSent == bytesTotal) {
            MetaData* ud = (MetaData*)(reply->userData(FILE_TAG));
            ud->LblStatus->setText("Optimizing...");
        }
    }
}


void TableWidget::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        if (bytesTotal <= 0) return;
        MetaData* ud = (MetaData*)(reply->userData(FILE_TAG));
        ud->ProgressBar->setValue(bytesReceived * 100 / bytesTotal);
    }
}


void TableWidget::onPostFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (handleError(reply)) {
        MetaData* ud = (MetaData*)(reply->userData(FILE_TAG));
        QByteArray location = reply->rawHeader("Location");        
        if (location.isEmpty()) {
            ud->setState(MetaData::Error);
            qDebug() << "Location header found empty for file " << ud->Filename;
        }
        else {
            QByteArray data = reply->readAll();
            TinyPNGPostOutput jOut; jOut.importFromJson(data);
            qint64 iSize = jOut.input()->size(), oSize = jOut.output()->size();
            double cSize = (qreal)(100.0 - (100.0 * oSize/iSize));
            QString ratioData = QString("%1/%2/%3\%").arg(d->formatSize(iSize)).arg(d->formatSize(oSize))
                    .arg(QString::number(cSize, 'f', 2));
            ud->LblCompressionInfo->setText(ratioData);
            ud->LblStatus->setText("Downloading...");
            ud->setState(MetaData::Downloading);
            QString filePath = location;
            QUrl url(filePath);
            QNetworkRequest req(url);
            QByteArray auth = QByteArray("Basic ") + QString("api:%1").arg(ADSM->data()->apiKey()).toLatin1().toBase64();
            req.setRawHeader("Authorization", auth);
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
        MetaData* ud = (MetaData*)(reply->userData(FILE_TAG));
        ud->setState(MetaData::Done);
        ud->LblStatus->setText("Done!");
        QByteArray data = reply->readAll();

        QString file;
        if (ADSM->data()->replaceSourceFile()) {
            file = ud->Filename;
        }
        else {
            int ind = ud->Filename.lastIndexOf('.');
            if (ind >= 0) {
                file = ud->Filename.insert(ind, ADSM->data()->fileFormat());
            }
            else {
                file = ud->Filename;
            }
        }

        QFile fp(file);
        if (fp.open(QFile::WriteOnly)) {
            fp.write(data);fp.close();
        }
        reply->deleteLater();
        if (ADSM->data()->removeItemWhenDone()) {
            for (int i = rowCount() - 1; i >= 0; i--) {
                QTableWidgetItem* twi = item(i, COL_FILE_NAME);
                MetaData* udRoot = (MetaData*)(twi->data(Qt::UserRole).value<void*>());
                if (udRoot == ud->Root) {
                    removeRow(i);
                    delete udRoot; break;
                }
            }
        }
    }
}
