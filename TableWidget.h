#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>

struct TableWidgetPri;
struct UserData;

class TableWidget : public QTableWidget
{
    Q_OBJECT
public:
    TableWidget(QWidget* p = 0);
    ~TableWidget();

protected:
    Qt::DropActions supportedDropActions() const;
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent* e);
    void dropEvent(QDropEvent* e);


private slots:
    void optimizeFile(UserData* ud);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onPostFinished();
    void onGetFinished();

private:
    TableWidgetPri* d;
};

#endif // TABLEWIDGET_H
