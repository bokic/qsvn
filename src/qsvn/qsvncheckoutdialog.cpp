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
    delete ui; ui = nullptr;
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
    m_TargetDir = dir;

    ui->lineEdit_dir->setText(m_TargetDir);
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

    dlg.setUrlLocations(QStringList() << location);

    if (dlg.exec() == QDialog::Accepted)
    {
        int selectedRevision = dlg.selectedRevision();

        if (selectedRevision > 0)
        {
            ui->radioButton_revision->setChecked(true);
            ui->lineEdit_revision->setEnabled(true);
            ui->lineEdit_revision->setText(QString::number(selectedRevision));
            ui->pushButton_ok->setEnabled(true);
        }
    }
}

void QSVNCheckoutDialog::on_lineEdit_dir_textChanged(const QString &text)
{
    QDir dir(text);

    if (text.isEmpty())
    {
        ui->lineEdit_dir->setStyleSheet("background: red");
        ui->lineEdit_dir->setToolTip(tr("Checkout directory is invalid."));

        return;
    }

    if (dir.exists())
    {
        QStringList files = dir.entryList();

        if (files.count() > 2)
        {
            ui->lineEdit_dir->setStyleSheet("background: red");
            ui->lineEdit_dir->setToolTip(tr("Checkout directory is not empty."));
        }
        else
        {
            ui->lineEdit_dir->setStyleSheet("");
            ui->lineEdit_dir->setToolTip("");
        }
    }
    else
    {
        if (dir.cdUp())
        {
            ui->lineEdit_dir->setStyleSheet("");
            ui->lineEdit_dir->setToolTip("");
        }
        else
        {
            QDir invalidDir = QDir(dir.absolutePath() + "/..");

            ui->lineEdit_dir->setStyleSheet("background: red");
            ui->lineEdit_dir->setToolTip(tr("Checkout directory(%1) doesn't exist.").arg(invalidDir.absolutePath()));
        }
    }
}

void QSVNCheckoutDialog::on_comboBox_URL_currentTextChanged(const QString &text)
{
    if ((!m_TargetDir.isEmpty())&&(!text.isEmpty()))
    {
        QUrl url(text);
        QString path = url.path();
        QStringList paths = path.split('/', QString::SkipEmptyParts);

        if (paths.last() == "trunk")
        {
            paths.removeLast();
        }

        QString newName = paths.last();
        newName.remove(QRegExp("\\.git$"));

        if (!newName.isEmpty())
        {
            ui->lineEdit_dir->setText(m_TargetDir + QDir::separator() + newName);
        }
    }
}
