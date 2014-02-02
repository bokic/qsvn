#include "qsvnupdatedialog.h"
#include "qsvnupdatetorevisiondialog.h"
#include "ui_qsvnupdatedialog.h"
#include "qsvncheckoutdialog.h"
#include "helpers.h"

#include <QCloseEvent>
#include <QtDebug>


QSVNUpdateDialog::QSVNUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNUpdateDialog),
    m_thread(this),
    m_depth(svn_depth_infinity),
    m_depth_is_sticky(true),
    m_ignore_externals(true),
    m_allow_unver_obstructions(true),
    m_add_as_modification(true),
    m_make_parents(true)
{
    ui->setupUi(this);

    connect(this, &QSVNUpdateDialog::update, m_thread.m_worker, &QSvn::update);
    connect(this, &QSVNUpdateDialog::checkout, m_thread.m_worker, &QSvn::checkout);

    // Notify signals
    connect(m_thread.m_worker, &QSvn::progress, this, &QSVNUpdateDialog::svnProgress, Qt::BlockingQueuedConnection);
    connect(m_thread.m_worker, &QSvn::notify, this, &QSVNUpdateDialog::svnNotify, Qt::BlockingQueuedConnection);
    connect(m_thread.m_worker, &QSvn::finished, this, &QSVNUpdateDialog::svnFinished, Qt::BlockingQueuedConnection);
    connect(m_thread.m_worker, &QSvn::error, this, &QSVNUpdateDialog::svnError, Qt::BlockingQueuedConnection);

    ui->tableWidget->setColumnWidth(0, 80);
    ui->tableWidget->setColumnWidth(1, 375);
    ui->tableWidget->setColumnWidth(2, 80);

    m_revision.kind = svn_opt_revision_head;

    m_thread.start();

}

QSVNUpdateDialog::~QSVNUpdateDialog()
{
    if (m_thread.isRunning())
    {
        m_thread.exit();

        m_thread.wait();
    }

    delete ui;
    ui = nullptr;
}

void QSVNUpdateDialog::setOperationUpdate(const QStringList &paths)
{
    if (!m_thread.m_worker->isBusy())
    {
        setWindowTitle(tr("Update"));

        m_paths = paths;

        emit update(m_paths, m_revision, m_depth, m_depth_is_sticky, m_ignore_externals, m_allow_unver_obstructions, m_add_as_modification, m_make_parents);
    }
    else
    {
        qDebug("QSVNUpdateDialog::setOperationXXX() worker thread is currently busy.");
    }
}

void QSVNUpdateDialog::setOperationUpdateToRevision(const QSVNUpdateToRevisionDialog &dlg)
{
    if (!m_thread.m_worker->isBusy())
    {
        setWindowTitle(tr("Update to revision"));

        m_paths = dlg.paths();
        m_revision = dlg.ui_revision();
        m_depth = dlg.ui_depth();
        m_ignore_externals = dlg.ui_include_externals();
        m_allow_unver_obstructions = dlg.ui_allow_unver();

        emit update(m_paths, m_revision, m_depth, m_depth_is_sticky, m_ignore_externals, m_allow_unver_obstructions, m_add_as_modification, m_make_parents);
    }
    else
    {
        qDebug("QSVNUpdateDialog::setOperationUpdateToRevision() worker thread is currently busy.");
    }
}

void QSVNUpdateDialog::setOperationCheckout(const QSVNCheckoutDialog &dlg)
{
    if (!m_thread.m_worker->isBusy())
    {
        setWindowTitle(tr("Checkout"));

        m_url = dlg.ui_url();
        m_path = dlg.ui_path();
        m_peg_revision = dlg.ui_revision(); // TODO: Check the initialization of m_peg_revision
        m_revision = dlg.ui_revision();
        m_depth = dlg.ui_depth();
        m_ignore_externals = dlg.ui_include_externals();
        m_allow_unver_obstructions = dlg.ui_allow_unver();

        emit checkout(m_url, m_path, m_peg_revision, m_revision, m_depth, m_ignore_externals, m_allow_unver_obstructions);
    }
    else
    {
        qDebug("QSVNUpdateDialog::setOperationCheckout() worker thread is currently busy.");
    }
}

void QSVNUpdateDialog::closeEvent(QCloseEvent *event)
{
    if (!event->isAccepted())
    {
        m_thread.m_worker->cancel();

        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void QSVNUpdateDialog::workerStarted()
{
}

void QSVNUpdateDialog::workerFinished()
{

}

void QSVNUpdateDialog::svnProgress(int progress, int total)
{
    if (total == -1)
    {
        ui->label1->setText(bytesToString(progress));
    }
}

void QSVNUpdateDialog::svnNotify(svn_wc_notify_t notify)
{
    int newRow;

    newRow = ui->tableWidget->rowCount();

    ui->tableWidget->insertRow(newRow);
    ui->tableWidget->setItem(newRow, 0, new QTableWidgetItem());
    ui->tableWidget->setItem(newRow, 1, new QTableWidgetItem());
    ui->tableWidget->setItem(newRow, 2, new QTableWidgetItem());

    ui->tableWidget->setRowHeight(newRow, ui->tableWidget->fontMetrics().height() + 4);

    ui->tableWidget->scrollToItem(ui->tableWidget->item(newRow, 0));

    switch(notify.action)
    {
    case svn_wc_notify_update_add:
        ui->tableWidget->item(newRow, 0)->setText(tr("add"));
        ui->tableWidget->item(newRow, 1)->setText(QString::fromUtf8(notify.path));
        break;
    case svn_wc_notify_update_completed:
        ui->tableWidget->item(newRow, 0)->setText(tr("completed"));
        break;
    case svn_wc_notify_update_delete:
        ui->tableWidget->item(newRow, 0)->setText(tr("delete"));
        ui->tableWidget->item(newRow, 1)->setText(QString::fromUtf8(notify.path));
        break;
    case svn_wc_notify_update_external:
        ui->tableWidget->item(newRow, 0)->setText(tr("external"));
        break;
    case svn_wc_notify_update_external_removed:
        ui->tableWidget->item(newRow, 0)->setText(tr("external removed"));
        break;
    case svn_wc_notify_update_replace:
        ui->tableWidget->item(newRow, 0)->setText(tr("replace"));
        ui->tableWidget->item(newRow, 1)->setText(QString::fromUtf8(notify.path));
        break;
    case svn_wc_notify_update_shadowed_add:
        ui->tableWidget->item(newRow, 0)->setText(tr("shadowed add"));
        break;
    case svn_wc_notify_update_shadowed_delete:
        ui->tableWidget->item(newRow, 0)->setText(tr("shadowed delete"));
        break;
    case svn_wc_notify_update_shadowed_update:
        ui->tableWidget->item(newRow, 0)->setText(tr("shadowed update"));
        break;
    case svn_wc_notify_update_skip_access_denied:
        ui->tableWidget->item(newRow, 0)->setText(tr("skip access denied"));
        break;
    case svn_wc_notify_update_skip_obstruction:
        ui->tableWidget->item(newRow, 0)->setText(tr("skip obstruction"));
        break;
    case svn_wc_notify_update_skip_working_only:
        ui->tableWidget->item(newRow, 0)->setText(tr("skip working only"));
        break;
    case svn_wc_notify_update_started:
        ui->tableWidget->item(newRow, 0)->setText(tr("started"));
        break;
    case svn_wc_notify_update_update:
        ui->tableWidget->item(newRow, 0)->setText(tr("update"));
        ui->tableWidget->item(newRow, 1)->setText(QString::fromUtf8(notify.path));
        break;
    default:
        ui->tableWidget->item(newRow, 0)->setText(tr("-- unimplemented action --")); // TODO: Implement missing action types.
        break;
    }
}

void QSVNUpdateDialog::svnFinished(bool result)
{
    Q_UNUSED(result);

    ui->pushButton_OK->setEnabled(true);
    ui->pushButton_Cancel->setEnabled(false);
    ui->pushButton_ShowLog->setEnabled(true);
}

void QSVNUpdateDialog::svnError(QString text)
{
    ui->label2->setText(text);
    ui->label2->setStyleSheet("background: red");
}
