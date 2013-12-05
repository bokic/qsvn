#include "qsvncheckoutdialog.h"
#include "ui_qsvncheckoutdialog.h"
#include "qsvnrepobrowserdialog.h"

#include <QSettings>


QSVNCheckoutDialog::QSVNCheckoutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNCheckoutDialog)
{
    ui->setupUi(this);

    loadSettings();

    ui->comboBox_URL->setCurrentIndex(-1);
}

QSVNCheckoutDialog::~QSVNCheckoutDialog()
{
    delete ui;
}

void QSVNCheckoutDialog::loadSettings()
{
    QSettings settings;

    settings.beginGroup("URL");

    for (int c = 1; settings.contains(QString("URL_%1").arg(c)); c++)
    {
        ui->comboBox_URL->addItem(settings.value(QString("URL_%1").arg(c)).toString());
    }

    settings.endGroup();
}

void QSVNCheckoutDialog::on_pushButton_URL_clicked()
{
    QSVNRepoBrowserDialog dlg(this);

    dlg.setURL(ui->comboBox_URL->currentText());

    if (dlg.exec() == QDialog::Accepted)
    {
        ui->comboBox_URL->setCurrentText(dlg.URL());
    }
}
