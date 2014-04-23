#include "qsvnlogindialog.h"
#include "ui_qsvnlogindialog.h"

QSVNLoginDialog::QSVNLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNLoginDialog)
{
    ui->setupUi(this);
}

QSVNLoginDialog::~QSVNLoginDialog()
{
    delete ui; ui = nullptr;
}

QString QSVNLoginDialog::username()
{
    return ui->username_lineEdit->text();
}

void QSVNLoginDialog::setUsername(const QString &username)
{
    ui->username_lineEdit->setText(username);

    if (username.isEmpty())
    {
        ui->username_lineEdit->setFocus();
    }
    else
    {
        ui->password_lineEdit->setFocus();
    }
}

QString QSVNLoginDialog::password()
{
    return ui->password_lineEdit->text();
}

void QSVNLoginDialog::setPassword(const QString &password)
{
    ui->password_lineEdit->setText(password);
}

bool QSVNLoginDialog::saveCredentials()
{
    return ui->savePassword_checkBox->isChecked();
}
