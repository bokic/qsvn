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
