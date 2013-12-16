#include "qsvnupdatetorevisiondialog.h"
#include "ui_qsvnupdatetorevisiondialog.h"

QSVNUpdateToRevisionDialog::QSVNUpdateToRevisionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNUpdateToRevisionDialog)
{
    ui->setupUi(this);
}

QSVNUpdateToRevisionDialog::~QSVNUpdateToRevisionDialog()
{
    delete ui;
}
