#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>

struct MainWindowPri;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void showSettingsDlg();
    void showAboutDlg();

private:
    MainWindowPri* d;

};

#endif // MAINWINDOW_H
