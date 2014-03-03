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
    delete ui;
}
