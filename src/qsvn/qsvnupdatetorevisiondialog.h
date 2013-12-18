#ifndef QSVNUPDATETOREVISIONDIALOG_H
#define QSVNUPDATETOREVISIONDIALOG_H

#include <QDialog>

#include "svn_client.h"
#include "svn_dirent_uri.h"
#include "svn_cmdline.h"
#include "svn_pools.h"
#include "svn_config.h"
#include "svn_fs.h"

namespace Ui {
class QSVNUpdateToRevisionDialog;
}

class QSVNUpdateToRevisionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSVNUpdateToRevisionDialog(QWidget *parent = 0);
    explicit QSVNUpdateToRevisionDialog(const QStringList &paths);
    ~QSVNUpdateToRevisionDialog();

    QStringList paths() const;
    svn_opt_revision_t ui_revision() const;
    svn_depth_t ui_depth() const;
    bool ui_include_externals() const;
    bool ui_allow_unver() const;

private slots:
    void on_revision_lineEdit_textChanged(const QString &arg1);

private:
    Ui::QSVNUpdateToRevisionDialog *ui;
    QStringList m_paths;
};

#endif // QSVNUPDATETOREVISIONDIALOG_H
