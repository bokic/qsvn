#include "qsvnupdatetorevisiondialog.h"
#include "ui_qsvnupdatetorevisiondialog.h"

QSVNUpdateToRevisionDialog::QSVNUpdateToRevisionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNUpdateToRevisionDialog)
{
    ui->setupUi(this);
}

QSVNUpdateToRevisionDialog::QSVNUpdateToRevisionDialog(const QStringList &paths) :
    QDialog(nullptr),
    ui(new Ui::QSVNUpdateToRevisionDialog)
{
    m_paths = paths;

    ui->setupUi(this);
}

QSVNUpdateToRevisionDialog::~QSVNUpdateToRevisionDialog()
{
    delete ui;
}

QStringList QSVNUpdateToRevisionDialog::paths() const
{
    return m_paths;
}

svn_opt_revision_t QSVNUpdateToRevisionDialog::ui_revision() const
{
    svn_opt_revision_t ret;

    if (ui->head_radioButton->isChecked())
    {
        ret.kind = svn_opt_revision_head;
    }
    else
    {
        ret.kind = svn_opt_revision_number;
        ret.value.number = ui->revision_lineEdit->text().toInt(); // TODO: Check for validity.
    }

    return ret;
}

svn_depth_t QSVNUpdateToRevisionDialog::ui_depth() const
{
    switch(ui->depth_comboBox->currentIndex())
    {
    case 0:
        return svn_depth_infinity; // TODO: Implement working copy update.
    case 1:
        return svn_depth_infinity;
    case 2:
        return svn_depth_immediates;
    case 3:
        return svn_depth_files;
    case 4:
        return svn_depth_empty;
    case 5:
        return svn_depth_exclude;
    default:
        return svn_depth_unknown;
    }
}

bool QSVNUpdateToRevisionDialog::ui_include_externals() const
{
    return !ui->omit_checkBox->isChecked();
}

bool QSVNUpdateToRevisionDialog::ui_allow_unver() const
{
    return true; // TODO: hardcoded return value for QSVNUpdateToRevisionDialog::ui_allow_unver()
}

void QSVNUpdateToRevisionDialog::on_revision_lineEdit_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty())
    {
        ui->revision_radioButton->setChecked(true);
    }
    else
    {
        ui->head_radioButton->setChecked(true);
    }
}
