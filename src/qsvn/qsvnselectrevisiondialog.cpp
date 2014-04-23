#include "qsvnselectrevisiondialog.h"
#include "ui_qsvnselectrevisiondialog.h"
#include "qsvnmessagelogdialog.h"

#include <QMessageBox>


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
    delete ui; ui = nullptr;
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

void QSVNSelectRevisionDialog::on_pushButton_ShowLog_clicked()
{
    QSVNMessageLogDialog dlg(this);

    if (m_locations.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"), tr("Repository URL is empty."));

        return;
    }

    dlg.setLocations(m_locations);

    if (dlg.exec() == QDialog::Accepted)
    {
        int selectedRevision = dlg.selectedRevision();

        if (selectedRevision > 0)
        {
            ui->lineEdit_Revision->setText(QString::number(selectedRevision));
        }
    }
}
