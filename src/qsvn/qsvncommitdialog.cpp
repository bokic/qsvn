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
    //connect(this, &QSVNCommitDialog::commit, m_thread.m_worker, &QSvn::commit);

    svn_opt_revision_t rev;
    rev.kind = svn_opt_revision_working;
    rev.value.number = 0;

    emit status(m_items.at(0), rev, svn_depth_infinity, false, false, false, true, true);
}

QSVNCommitDialog::~QSVNCommitDialog()
{
    delete ui;
}

void QSVNCommitDialog::statusFinished(QList<QSvnStatusItem> items, bool error)
{
    ui->changes_tableView->setModel(new QSVNCommitItemsModel(items, m_commonDir));
}
