#include "MainWindow.h"
#include "TableWidget.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "SettingsDlg.h"

struct MainWindowPri{
    TableWidget* TableUI;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), d(new MainWindowPri)
{
    setWindowTitle("TinyPNG Client");
    d->TableUI = new TableWidget;
    QWidget * w = new QWidget;
    QHBoxLayout* hboxRoot = new QHBoxLayout(w);
    hboxRoot->addWidget(d->TableUI, 1);
    QVBoxLayout* hbox = new QVBoxLayout;
    hbox->setMargin(0);
    QPushButton* pbStart = new QPushButton("Start");
    hbox->addWidget(pbStart);hbox->addStretch(1);
    QPushButton* pbClear = new QPushButton("Clear Items");
    QPushButton* pbSettings = new QPushButton("Settings");
    hbox->addWidget(pbClear);hbox->addWidget(pbSettings);
    hboxRoot->addLayout(hbox);
    setCentralWidget(w);
    resize(1024, 400);

    connect(pbClear, SIGNAL(clicked(bool)), d->TableUI, SLOT(clearItems()));
    connect(pbStart, SIGNAL(clicked(bool)), d->TableUI, SLOT(startOptimizing()));
    connect(pbSettings, SIGNAL(clicked(bool)), SLOT(showSettingsDlg()));
}

MainWindow::~MainWindow () {delete d;}


void MainWindow::showSettingsDlg()
{
    SettingsDlg *dlg = new SettingsDlg;
    dlg->exec();
}

