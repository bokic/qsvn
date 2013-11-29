#include "qsvncommitdialog.h"
#include "ui_qsvncommitdialog.h"

QSVNCommitDialog::QSVNCommitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNCommitDialog)
{
    ui->setupUi(this);
}

QSVNCommitDialog::~QSVNCommitDialog()
{
    delete ui;
}
