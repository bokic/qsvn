#include "qsvncommitdialog.h"
#include "ui_qsvncommitdialog.h"

QSVNCommitDialog::QSVNCommitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNCommitDialog),
    m_operation(QSvn::QSVNOperationNone)
{
    ui->setupUi(this);

    connect(&m_thread, &QThread::started, this, &QSVNCommitDialog::workerStarted);
}

QSVNCommitDialog::~QSVNCommitDialog()
{
    delete ui;
}

void QSVNCommitDialog::setOperationStatus(const QStringList &paths)
{
    if (m_operation == QSvn::QSVNOperationNone)
    {
        m_operation = QSvn::QSVNOperationCommit;
        m_paths = paths;

        m_thread.start();
        m_thread.waitForStartup();
    }
    else
    {
        qDebug("QSVNUpdateDialog::setOperationXXX() called more than once.");
    }
}

void QSVNCommitDialog::workerStarted()
{

}
