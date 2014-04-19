#include "qsvncheckoutdialog.h"
#include "ui_qsvncheckoutdialog.h"
#include "qsvnrepobrowserdialog.h"
#include "qsvnmessagelogdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDir>


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

    ui->comboBox_URL->addItem("https://github.com/bokic/qsvn/trunk");

    settings.endGroup();
}

void QSVNCheckoutDialog::setTargetDir(const QString &dir)
{
    ui->lineEdit_dir->setText(dir);
}

void QSVNCheckoutDialog::on_pushButton_URL_clicked()
{
    QSVNRepoBrowserDialog dlg(this);

	QString curURL = ui->comboBox_URL->currentText();

	if (!curURL.isEmpty())
	{
		dlg.setURL(ui->comboBox_URL->currentText());
	}

    if (dlg.exec() == QDialog::Accepted)
    {
        ui->comboBox_URL->setCurrentText(dlg.URL());
    }
}

QString QSVNCheckoutDialog::ui_url() const
{
    return ui->comboBox_URL->currentText();
}

QString QSVNCheckoutDialog::ui_path() const
{
    return ui->lineEdit_dir->text();
}

svn_opt_revision_t QSVNCheckoutDialog::ui_revision() const
{
    svn_opt_revision_t ret;

    if (ui->radioButton_head->isChecked())
    {
        ret.kind = svn_opt_revision_head;
    }
    else
    {
        ret.kind = svn_opt_revision_number;
        ret.value.number = ui->lineEdit_revision->text().toInt(); // TODO: Check for validity.
    }

    return ret;
}

svn_depth_t QSVNCheckoutDialog::ui_depth() const
{
    switch(ui->comboBox_depth->currentIndex())
    {
    case 0:
        return svn_depth_infinity;
    case 1:
        return svn_depth_immediates;
    case 2:
        return svn_depth_files;
    case 3:
        return svn_depth_empty;
    default:
        return svn_depth_unknown;
    }
}

svn_boolean_t QSVNCheckoutDialog::ui_include_externals() const
{
    return ui->checkBox_omit->isChecked();
}

svn_boolean_t QSVNCheckoutDialog::ui_allow_unver() const
{
    return true; // TODO: Implement QSVNCheckoutDialog::ui_allow_unver()
}

void QSVNCheckoutDialog::on_pushButton_dir_clicked()
{
    QString dir;

    dir = ui->lineEdit_dir->text();

    if (dir.isEmpty())
    {
        dir = QDir::currentPath();
    }

    dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        ui->lineEdit_dir->setText(dir);
    }
}

void QSVNCheckoutDialog::on_radioButton_head_clicked()
{
    ui->lineEdit_revision->setEnabled(false);
}

void QSVNCheckoutDialog::on_radioButton_revision_clicked()
{
    ui->lineEdit_revision->setEnabled(true);
    ui->lineEdit_revision->setFocus();
    ui->lineEdit_revision->selectAll();
}

void QSVNCheckoutDialog::on_pushButton_revision_clicked()
{
    QSVNMessageLogDialog dlg(this);
    QString location;

    location = ui->comboBox_URL->currentText();

    if (location.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"), tr("Repository URL is empty."));
        ui->comboBox_URL->setFocus();

        return;
    }

    dlg.setLocation(location);

    if (dlg.exec() == QDialog::Accepted)
    {
        ui->radioButton_revision->setChecked(true);
        ui->lineEdit_revision->setEnabled(true);
        ui->lineEdit_revision->setText("some rev. number");
        ui->lineEdit_revision->setStyleSheet("background: red");
        ui->lineEdit_revision->setToolTip("Invalid revision number.");
        ui->pushButton_ok->setEnabled(false);
    }
}
