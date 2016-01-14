#include "MainWindow.h"
#include "TableWidget.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("TinyPNG Client");
    setCentralWidget(new TableWidget);
    resize(1024, 400);
}




