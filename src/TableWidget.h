#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>
#include "JObject.h"
#include <QLabel>
#include <QProgressBar>

class TinyPNGError : public JObject {
    Q_OBJECT
public:
    Q_INVOKABLE explicit TinyPNGError(QObject* parent = 0) : JObject(parent){}
    MetaPropertyPrivateSet(QString, error)
    MetaPropertyPrivateSet(QString, message)
};


class ImageInput : public JObject {
    Q_OBJECT
public:
    Q_INVOKABLE explicit ImageInput(QObject* parent = 0) : JObject(parent){}

    MetaPropertyPrivateSet(qint64, size)
    MetaPropertyPrivateSet(QString, type)
};

class ImageOutput : public ImageInput {
    Q_OBJECT
public:
    Q_INVOKABLE explicit ImageOutput(QObject* parent = 0) : ImageInput(parent){}

    MetaPropertyPrivateSet(int, width)
    MetaPropertyPrivateSet(int, height)
    MetaPropertyPrivateSet(qreal, ratio)
    MetaPropertyPrivateSet(QString, url)
};

class TinyPNGPostOutput : public JObject {
    Q_OBJECT
public:
    Q_INVOKABLE explicit TinyPNGPostOutput(QObject* parent = 0) : JObject(parent){}
    MetaPropertyPrivateSet_Ptr(ImageInput, input)
    MetaPropertyPrivateSet_Ptr(ImageOutput, output)
};


struct TableWidgetPri;
struct MetaData;
class QNetworkReply;

class TableWidget : public QTableWidget
{
    Q_OBJECT
public:
    TableWidget(QWidget* p = 0);
    ~TableWidget();

public slots:
    void startOptimizing();
    void clearItems();

protected:
    Qt::DropActions supportedDropActions() const;
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent* e);
    void dropEvent(QDropEvent* e);


private slots:
    void optimizeFile(MetaData* ud);
    void onUploadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onPostFinished();
    void onGetFinished();


private:
    bool handleError(QNetworkReply* reply);


private:
    TableWidgetPri* d;
};

#endif // TABLEWIDGET_H
