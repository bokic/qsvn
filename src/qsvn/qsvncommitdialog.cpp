#include "qsvncommitdialog.h"
#include "ui_qsvncommitdialog.h"
#include "qsvncommititemsmodel.h"

QSVNCommitDialog::QSVNCommitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNCommitDialog),
    m_operation(QSvn::QSVNOperationNone)
{
    ui->setupUi(this);

    m_thread.start();
    m_thread.waitForStartup();

    connect(this, &QSVNCommitDialog::status, m_thread.m_worker, &QSvn::status);
    connect(m_thread.m_worker, &QSvn::statusFinished, this, &QSVNCommitDialog::statusFinished, Qt::BlockingQueuedConnection);
    //connect(this, &QSVNCommitDialog::commit, m_thread.m_worker, &QSvn::commit);
}

QSVNCommitDialog::~QSVNCommitDialog()
{
    delete ui;
}

void QSVNCommitDialog::setOperationStatus(const QString &path)
{
    if (!m_thread.m_worker->isBusy())
    {
        svn_opt_revision_t rev;
        rev.kind = svn_opt_revision_working;
        rev.value.number = 0;

        emit status(path, rev, svn_depth_infinity, false, false, false, true, true);
    }
    else
    {
        qDebug("QSVNCommitDialog::setOperationStatus() worker thread is currently busy.");
    }
}

void QSVNCommitDialog::statusFinished(QList<QSvnStatusItem> items, bool error)
{
    ui->changes_tableView->setModel(new QSVNCommitItemsModel(items));
}
