#include "qsvncleanupdialog.h"
#include "ui_qsvncleanupdialog.h"

#include "svn_client.h"
#include "svn_dirent_uri.h"
#include "svn_cmdline.h"
#include "svn_pools.h"
#include "svn_config.h"
#include "svn_fs.h"


QSvnCleanupDialog::QSvnCleanupDialog(const QStringList &paths, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSvnCleanupDialog),
    m_paths(paths)
{
    ui->setupUi(this);
}

QSvnCleanupDialog::~QSvnCleanupDialog()
{
    delete ui;
}

void QSvnCleanupDialog::accept()
{
    QDialog::accept();

    //svn_wc_cleanup3();
}
