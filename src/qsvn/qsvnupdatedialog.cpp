#include "qsvnupdatedialog.h"
#include "ui_qsvnupdatedialog.h"
#include "qsvncheckoutdialog.h"

#include <QtDebug>


QSVNUpdateDialog::QSVNUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNUpdateDialog),
    m_thread(this),
    m_depth(svn_depth_infinity),
    m_operation(QSVNOperationNone)
{
    ui->setupUi(this);

    m_revision.kind = svn_opt_revision_head;

    connect(&m_thread, &QThread::started, this, &QSVNUpdateDialog::workerStarted);
    connect(&m_thread, &QThread::finished, this, &QSVNUpdateDialog::workerFinished);
}

QSVNUpdateDialog::~QSVNUpdateDialog()
{
    if (m_thread.isRunning())
    {
        m_thread.exit();

        m_thread.wait();
    }

    delete ui;
}

void QSVNUpdateDialog::setOperationUpdate(const QStringList &paths)
{
    if (m_operation == QSVNOperationNone)
    {
        m_operation = QSVNOperationUpdate;
        m_paths = paths;

        m_thread.start();
    }
    else
    {
        qDebug("QSVNUpdateDialog::setOperationXXX() called more than once.");
    }
}

void QSVNUpdateDialog::setOperationCheckout(const QSVNCheckoutDialog &dlg)
{
    if (m_operation == QSVNOperationNone)
    {
        m_operation = QSVNOperationCheckout;

        m_url = dlg.ui_url();
        m_path = dlg.ui_path();
        m_peg_revision = dlg.ui_revision(); // TODO: Check the initialization of m_peg_revision
        m_revision = dlg.ui_revision();
        m_depth = dlg.ui_depth();
        m_ignore_externals = dlg.ui_include_externals();
        m_allow_unver_obstructions = dlg.ui_allow_unver();

        m_thread.start();
    }
    else
    {
        qDebug("QSVNUpdateDialog::setOperationXXX() called more than once.");
    }
}

void QSVNUpdateDialog::workerStarted()
{
    switch(m_operation)
    {
    case QSVNOperationCommit:
        break;
    case QSVNOperationUpdate:
        //connect(m_thread.m_worker, &QSvn::update, this, &QSVNRepoBrowserDialog::workerResult);
        //connect(m_thread.m_worker, &QSvn::error, this, &QSVNUpdateDialog::workerError);
        connect(this, &QSVNUpdateDialog::update, m_thread.m_worker, &QSvn::update);

        emit update(m_paths, m_revision, m_depth, true, true, true, true, true);
        break;
    case QSVNOperationUpdateToRevision:
        break;
    case QSVNOperationCheckout:
        connect(this, &QSVNUpdateDialog::checkout, m_thread.m_worker, &QSvn::checkout);
        connect(m_thread.m_worker, &QSvn::progress, this, &QSVNUpdateDialog::svnProgress, Qt::BlockingQueuedConnection);
        connect(m_thread.m_worker, &QSvn::notify, this, &QSVNUpdateDialog::svnNotify, Qt::BlockingQueuedConnection);
        connect(m_thread.m_worker, &QSvn::finished, this, &QSVNUpdateDialog::svnFinished, Qt::BlockingQueuedConnection);
        connect(m_thread.m_worker, &QSvn::error, this, &QSVNUpdateDialog::svnError, Qt::BlockingQueuedConnection);

        emit checkout(m_url, m_path, m_peg_revision, m_revision, m_depth, m_ignore_externals, m_allow_unver_obstructions);
        break;
    default:
        break;
    }
}

void QSVNUpdateDialog::workerFinished()
{
    Q_UNIMPLEMENTED();

    //close();
}

void QSVNUpdateDialog::svnProgress(int progress, int total)
{
    Q_UNUSED(total);

    ui->progress->setText(tr("Progress: %1 bytes.").arg(progress));
}

void QSVNUpdateDialog::svnNotify(svn_wc_notify_t notify)
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem());
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, new QTableWidgetItem());
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, new QTableWidgetItem());

    ui->tableWidget->scrollToItem(ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 0));

    switch(notify.action)
    {
    case svn_wc_notify_update_started:
        ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 0)->setText(tr("checkout"));
        break;
    case svn_wc_notify_update_add:
        ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 0)->setText(tr("add"));
        ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 1)->setText(QString::fromUtf8(notify.path));
        break;
    case svn_wc_notify_update_update:
        ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 0)->setText(tr("update"));
        ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 1)->setText(QString::fromUtf8(notify.path));
        break;
    case svn_wc_notify_update_completed:
        ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 0)->setText(tr("done"));
        break;
    default:
        ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 0)->setText(tr("???"));
        break;
    }
}

void QSVNUpdateDialog::svnFinished(bool result)
{
    Q_UNUSED(result);

    ui->progress->setText(tr("Finished."));

    ui->pushButton_OK->setEnabled(true);
    ui->pushButton_Cancel->setEnabled(false);
}

void QSVNUpdateDialog::svnError(QString text)
{
    Q_UNUSED(text);

    Q_UNIMPLEMENTED();
}
