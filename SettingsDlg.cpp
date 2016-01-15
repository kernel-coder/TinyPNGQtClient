#include "SettingsDlg.h"
#include "ui_SettingsDlg.h"
#include "AppSettingsManager.h"
#include <QMessageBox>

SettingsDlg::SettingsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDlg)
{
    ui->setupUi(this);

    connect(ui->cbReplaceFile, SIGNAL(toggled(bool)), SLOT(onReplaceSourceOptionToggled(bool)));

    ui->leAPIUrl->setText(ADSM->data()->apiUrl());
    ui->leAPIKey->setText(ADSM->data()->apiKey());
    ui->cbRemoveItem->setChecked(ADSM->data()->removeItemWhenDone());
    ui->cbReplaceFile->setChecked(ADSM->data()->replaceSourceFile());
    ui->leFileFormat->setText(ADSM->data()->fileFormat());
    ui->cbStartOptimingWhenDrop->setChecked(ADSM->data()->startsOptimizingWhenDrop());
    ui->cbRemoveErrorItems->setChecked(ADSM->data()->removeErrorItems());

    connect(ui->btnOk, SIGNAL(clicked(bool)), SLOT(onSave()));
    connect(ui->btnCancel, SIGNAL(clicked(bool)), SLOT(reject()));
}


SettingsDlg::~SettingsDlg()
{
    delete ui;
}


void SettingsDlg::onReplaceSourceOptionToggled(bool checked)
{
    ui->leFileFormat->setEnabled(!checked);
}


void SettingsDlg::onSave()
{
    if (ui->leAPIKey->text().trimmed().isEmpty() == false && ui->leAPIUrl->text().trimmed().isEmpty() == false) {
        ADSM->data()->apiUrl(ui->leAPIUrl->text().trimmed());
        ADSM->data()->apiKey(ui->leAPIKey->text().trimmed());
        ADSM->data()->replaceSourceFile(ui->cbReplaceFile->isChecked());
        ADSM->data()->removeItemWhenDone(ui->cbRemoveItem->isChecked());
        ADSM->data()->fileFormat(ui->leFileFormat->text());
        ADSM->data()->startsOptimizingWhenDrop(ui->cbStartOptimingWhenDrop->isChecked());
        ADSM->data()->removeErrorItems(ui->cbRemoveErrorItems->isChecked());
        ADSM->save();
        accept();
    }
    else {
        QMessageBox::warning(this, "Wrong API Info", "Please provide correct API url and API key.");
    }
}
