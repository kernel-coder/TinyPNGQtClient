#include "MainWindow.h"
#include "TableWidget.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "SettingsDlg.h"
#include "AboutDlg.h"

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
    QVBoxLayout* vbox = new QVBoxLayout; vbox->setMargin(0);
    QPushButton* pbStart = new QPushButton("Start");
    vbox->addWidget(pbStart);vbox->addStretch(1);
    QPushButton* pbClear = new QPushButton("Clear Items");
    QPushButton* pbSettings = new QPushButton("Settings");
    vbox->addWidget(pbClear);vbox->addWidget(pbSettings);
    QPushButton* pbAbout = new QPushButton("About");
    vbox->addWidget(pbAbout);
    hboxRoot->addLayout(vbox);
    setCentralWidget(w);
    resize(1024, 400);

    connect(pbClear, SIGNAL(clicked(bool)), d->TableUI, SLOT(clearItems()));
    connect(pbStart, SIGNAL(clicked(bool)), d->TableUI, SLOT(startOptimizing()));
    connect(pbSettings, SIGNAL(clicked(bool)), SLOT(showSettingsDlg()));
    connect(pbAbout, SIGNAL(clicked(bool)), SLOT(showAboutDlg()));
}

MainWindow::~MainWindow () {delete d;}


void MainWindow::showSettingsDlg()
{
    SettingsDlg *dlg = new SettingsDlg(this);
    dlg->exec();
}


void MainWindow::showAboutDlg()
{
    AboutDlg *dlg = new AboutDlg(this);
    dlg->exec();
}

