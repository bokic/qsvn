#include "qsvnupdatedialog.h"
#include "ui_qsvnupdatedialog.h"

QSVNUpdateDialog::QSVNUpdateDialog(QStringList paths, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNUpdateDialog),
    m_thread(this),
    m_paths(paths),
    m_depth(svn_depth_infinity)
{
    ui->setupUi(this);

    m_revision.kind = svn_opt_revision_head;

    connect(&m_thread, &QThread::started, this, &QSVNUpdateDialog::workerStarted);
    connect(&m_thread, &QThread::finished, this, &QSVNUpdateDialog::workerFinished);

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
}

void QSVNUpdateDialog::workerStarted()
{
    //connect(m_thread.m_worker, &QSvn::update, this, &QSVNRepoBrowserDialog::workerResult);
    //connect(m_thread.m_worker, &QSvn::error, this, &QSVNUpdateDialog::workerError);
    connect(this, &QSVNUpdateDialog::update, m_thread.m_worker, &QSvn::update);

    emit update(m_paths, m_revision, m_depth, true, true, true, true, true);
}

void QSVNUpdateDialog::workerFinished()
{
    //close();
}
