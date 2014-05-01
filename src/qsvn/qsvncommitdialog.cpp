#include "qsvncommitdialog.h"
#include "ui_qsvncommitdialog.h"
#include "qsvncommititemsmodel.h"
#include "qsvnmessagelogdialog.h"
#include "qsvncommithistorydialog.h"
#include "qsvnhistory.h"
#include "helpers.h"

#include <QMessageBox>
#include <QDebug>


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

    connect(ui->allFiles_label, &QClickableLabel::clicked, this, &QSVNCommitDialog::label_clicked);
    connect(ui->noneFiles_label, &QClickableLabel::clicked, this, &QSVNCommitDialog::label_clicked);
    connect(ui->nonVersionedFiles_label, &QClickableLabel::clicked, this, &QSVNCommitDialog::label_clicked);
    connect(ui->versionedFiles_label, &QClickableLabel::clicked, this, &QSVNCommitDialog::label_clicked);
    connect(ui->addedFiles_label, &QClickableLabel::clicked, this, &QSVNCommitDialog::label_clicked);
    connect(ui->deletedFiles_label, &QClickableLabel::clicked, this, &QSVNCommitDialog::label_clicked);
    connect(ui->modifiedFiles_label, &QClickableLabel::clicked, this, &QSVNCommitDialog::label_clicked);
    connect(ui->filesFiles_label, &QClickableLabel::clicked, this, &QSVNCommitDialog::label_clicked);
    connect(ui->directoriesFiles_label, &QClickableLabel::clicked, this, &QSVNCommitDialog::label_clicked);

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

QString QSVNCommitDialog::ui_message() const
{
    return ui->message_plainTextEdit->toPlainText();
}

void QSVNCommitDialog::statusFinished(QList<QSvnStatusItem> items, QSvnError error)
{
    Q_UNUSED(error);

    removeNormalSvnFiles(items);

    ui->changes_tableView->setModel(new QSVNCommitItemsModel(items, m_commonDir));

    connect((QSVNCommitItemsModel *)ui->changes_tableView->model(), &QSVNCommitItemsModel::checked, this, &QSVNCommitDialog::filesChecked);

    updateTotalAndChecked();
    updateLabelsState();
}

void QSVNCommitDialog::on_showUnversioned_checkBox_stateChanged(int state)
{
    ((QSVNCommitItemsModel *)ui->changes_tableView->model())->showUnversionedFiles(state?true:false);

    updateTotalAndChecked();
    updateLabelsState();
}

void QSVNCommitDialog::on_showLog_pushButton_clicked()
{
    QSVNMessageLogDialog dlg(this);
    QSvn svn;

    svn.init();

    dlg.setUrlLocations(QStringList() << svn.urlFromPath(m_commonDir));
    dlg.exec();
}

void QSVNCommitDialog::updateTotalAndChecked()
{
    int total = ((QSVNCommitItemsModel *)ui->changes_tableView->model())->totalItemCount();
    int checked = ((QSVNCommitItemsModel *)ui->changes_tableView->model())->checkItemCount();

    ui->selectedTotal_label->setText(tr("%1 files selected, %2 files total").arg(checked).arg(total));
}

void QSVNCommitDialog::removeNormalSvnFiles(QList<QSvnStatusItem> &items)
{
    for (int c = items.count() - 1; c >= 0; c--)
    {
        if (items[c].m_nodeStatus == svn_wc_status_normal)
        {
            items.removeAt(c);
        }
    }
}

void QSVNCommitDialog::filesChecked(int index, bool state)
{
    Q_UNUSED(index);
    Q_UNUSED(state);

    updateTotalAndChecked();
}

void QSVNCommitDialog::label_clicked()
{
    QSVNCommitItemsModel *model = (QSVNCommitItemsModel*)ui->changes_tableView->model();
    QClickableLabel *label = (QClickableLabel *)sender();

    if (label == ui->allFiles_label)
    {
        model->checkAllItems();
    }
    else if (label == ui->noneFiles_label)
    {
        model->uncheckAllItems();
    }
    else if (label == ui->nonVersionedFiles_label)
    {
        model->checkNonVersionedItems();
    }
    else if (label == ui->versionedFiles_label)
    {
        model->checkVersionedItems();
    }
    else if (label == ui->addedFiles_label)
    {
        model->checkAddedItems();
    }
    else if (label == ui->deletedFiles_label)
    {
        model->checkDeletedItems();
    }
    else if (label == ui->modifiedFiles_label)
    {
        model->checkModifiedItems();
    }
    else if (label == ui->filesFiles_label)
    {
        model->checkFileItems();
    }
    else if (label == ui->directoriesFiles_label)
    {
        model->checkDirItems();
    }
    else
    {
        qDebug() << "Unknown clickable label.";
    }
}

void QSVNCommitDialog::updateLabelsState()
{
    QList<QSvnStatusItem> items;
    bool has_non_versioned = false;
    bool has_versioned = false;
    bool has_added = false;
    bool has_deleted = false;
    bool has_modified = false;
    bool has_file = false;
    bool has_dir = false;

    items = ((QSVNCommitItemsModel *)ui->changes_tableView->model())->items();

    if (items.count() == 0)
    {
        ui->allFiles_label->setEnabled(false);
        ui->noneFiles_label->setEnabled(false);
        ui->nonVersionedFiles_label->setEnabled(false);
        ui->versionedFiles_label->setEnabled(false);
        ui->addedFiles_label->setEnabled(false);
        ui->deletedFiles_label->setEnabled(false);
        ui->modifiedFiles_label->setEnabled(false);
        ui->filesFiles_label->setEnabled(false);
        ui->directoriesFiles_label->setEnabled(false);

        return;
    }

    ui->allFiles_label->setEnabled(true);
    ui->noneFiles_label->setEnabled(true);

    foreach(const QSvnStatusItem &item, items)
    {
        if (item.m_nodeStatus == svn_wc_status_unversioned)
        {
            has_non_versioned = true;
        }
        else
        {
            has_versioned = true;
        }

        if (item.m_nodeStatus == svn_wc_status_added)
        {
            has_added = true;
        }
        else if (item.m_nodeStatus == svn_wc_status_missing)
        {
            has_deleted = true;
        }
        else if (item.m_nodeStatus == svn_wc_status_modified)
        {
            has_modified = true;
        }
        else if (item.m_kind == svn_node_file)
        {
            has_file = true;
        }
        else if (item.m_kind == svn_node_dir)
        {
            has_dir = true;
        }
        else if (item.m_kind == svn_node_unknown)
        {
            if (QFileInfo(item.m_filename).isFile())
            {
                has_file = true;
            }
            else if (QFileInfo(item.m_filename).isDir())
            {
                has_dir = true;
            }
            else
            {
                qDebug() << "Unknown item type.";
            }
        }
    }

    if (has_non_versioned)
    {
        ui->nonVersionedFiles_label->setEnabled(true);
    }
    else
    {
        ui->nonVersionedFiles_label->setEnabled(false);
    }

    if (has_versioned)
    {
        ui->versionedFiles_label->setEnabled(true);
    }
    else
    {
        ui->versionedFiles_label->setEnabled(false);
    }

    if (has_added)
    {
        ui->addedFiles_label->setEnabled(true);
    }
    else
    {
        ui->addedFiles_label->setEnabled(false);
    }

    if (has_deleted)
    {
        ui->deletedFiles_label->setEnabled(true);
    }
    else
    {
        ui->deletedFiles_label->setEnabled(false);
    }

    if (has_modified)
    {
        ui->modifiedFiles_label->setEnabled(true);
    }
    else
    {
        ui->modifiedFiles_label->setEnabled(false);
    }

    if (has_file)
    {
        ui->filesFiles_label->setEnabled(true);
    }
    else
    {
        ui->filesFiles_label->setEnabled(false);
    }

    if (has_dir)
    {
        ui->directoriesFiles_label->setEnabled(true);
    }
    else
    {
        ui->directoriesFiles_label->setEnabled(false);
    }
}

void QSVNCommitDialog::on_rescentMessage_toolButton_clicked()
{
    QSvnCommitHistoryDialog dlg(this);

    QSvnHistory historyLog(m_thread.m_worker->urlFromPath(m_commonDir));

    dlg.setItems(historyLog.items());

    if (dlg.exec() == QDialog::Accepted)
    {
        ui->message_plainTextEdit->setPlainText(dlg.selectedItem());
    }
}

void QSVNCommitDialog::on_QSVNCommitDialog_accepted()
{
    const QString &messageLog = ui->message_plainTextEdit->toPlainText();

    if (!messageLog.isEmpty())
    {
        QSvnHistory historyLog(m_thread.m_worker->urlFromPath(m_commonDir));

        historyLog.add(messageLog);
    }
}
