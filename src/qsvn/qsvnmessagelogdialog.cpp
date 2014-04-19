#include "qsvnmessagelogdialog.h"
#include "ui_qsvnmessagelogdialog.h"

QSVNMessageLogDialog::QSVNMessageLogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNMessageLogDialog)
{
    ui->setupUi(this);
}

QSVNMessageLogDialog::~QSVNMessageLogDialog()
{
    delete ui;
}

void QSVNMessageLogDialog::setLocation(const QString &location)
{
    m_location = location;
}