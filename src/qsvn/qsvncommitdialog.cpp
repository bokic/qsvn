#include "qsvncommitdialog.h"
#include "ui_qsvncommitdialog.h"
#include "qsvncommititemsmodel.h"
#include "helpers.h"


QSVNCommitDialog::QSVNCommitDialog(const QStringList &items, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QSVNCommitDialog)
    , m_items(items)
    , m_operation(QSvn::QSVNOperationNone)
{
    ui->setupUi(this);

    ui->changes_tableView->verticalHeader()->setDefaultSectionSize(ui->changes_tableView->fontMetrics().height() + 4);


    m_thread.start();
    m_thread.waitForStartup();

    m_commonDir = getCommonDir(m_items);

    setWindowTitle(tr("%1 - Commit - QSvn").arg(m_commonDir));

    if (m_items.length() == 1)
    {
        ui->commitURL_label->setText(m_thread.m_worker->urlFromPath(m_commonDir));
    }
    else
    {
        ui->commitURL_label->setText(tr("(multiple targets selected)"));
    }

    connect(this, &QSVNCommitDialog::status, m_thread.m_worker, &QSvn::status);
    connect(m_thread.m_worker, &QSvn::statusFinished, this, &QSVNCommitDialog::statusFinished, Qt::BlockingQueuedConnection);

    svn_opt_revision_t rev;
    rev.kind = svn_opt_revision_working;
    rev.value.number = 0;

    emit status(m_items.at(0), rev, svn_depth_infinity, true, false, false, true, true);
}

QSVNCommitDialog::~QSVNCommitDialog()
{
    emit m_thread.quit();
    m_thread.wait();

    delete ui; ui = nullptr;
}

QStringList QSVNCommitDialog::ui_checked_path_items() const
{
    QSVNCommitItemsModel *model = (QSVNCommitItemsModel *)ui->changes_tableView->model();
    QStringList ret;

    const QList<QSvnStatusItem> &items = model->checkedItems();

    foreach(const QSvnStatusItem &item, items)
    {
        ret.append(item.m_filename);
    }

    return ret;
}

svn_depth_t QSVNCommitDialog::ui_depth() const
{
    return svn_depth_infinity; // TODO Implement me bool QSVNCommitDialog::ui_depth() const
}

bool QSVNCommitDialog::ui_keep_locks() const
{
    if (ui->keepLocks_checkBox->isChecked())
    {
        return true;
    }

    return false;
}

bool QSVNCommitDialog::ui_changelist() const
{
    return true; // TODO Implement me bool QSVNCommitDialog::ui_changelist() const
}

bool QSVNCommitDialog::ui_m_commit_as_operations() const
{
    return true; // TODO Implement me bool QSVNCommitDialog::ui_m_commit_as_operations() const
}

void QSVNCommitDialog::statusFinished(QList<QSvnStatusItem> items, bool error)
{
    ui->changes_tableView->setModel(new QSVNCommitItemsModel(items, m_commonDir));
}

void QSVNCommitDialog::on_showUnversioned_checkBox_stateChanged(int state)
{
    ((QSVNCommitItemsModel *)ui->changes_tableView->model())->showUnversionedFiles(state?true:false);
}
