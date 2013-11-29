#include "qsvnselectrevisiondialog.h"
#include "ui_qsvnselectrevisiondialog.h"

QSVNSelectRevisionDialog::QSVNSelectRevisionDialog(QWidget *parent, const QString &text) :
    QDialog(parent),
    ui(new Ui::QSVNSelectRevisionDialog)
{
    ui->setupUi(this);

    if (text != tr("HEAD"))
    {
        ui->radioButton_Revision->setChecked(true);
        ui->lineEdit_Revision->setText(text);
        ui->lineEdit_Revision->setFocus();
    }
}

QSVNSelectRevisionDialog::~QSVNSelectRevisionDialog()
{
    delete ui;
}

int QSVNSelectRevisionDialog::revision()
{
    if (ui->radioButton_Revision->isChecked())
    {
        const QString &revStr = ui->lineEdit_Revision->text();

        if (!revStr.isEmpty())
        {
            return revStr.toInt();
        }
    }

    return -1;
}

void QSVNSelectRevisionDialog::on_lineEdit_Revision_editingFinished()
{
    ui->radioButton_Revision->setChecked(true);
}
