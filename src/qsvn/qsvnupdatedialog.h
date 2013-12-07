#ifndef QSVNUPDATEDIALOG_H
#define QSVNUPDATEDIALOG_H

#include "qsvn.h"
#include "qsvnthread.h"

#include <QStringList>
#include <QDialog>

namespace Ui {
class QSVNUpdateDialog;
}

class QSVNCheckoutDialog;

class QSVNUpdateDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QSVNUpdateDialog(QWidget *parent = 0);
    ~QSVNUpdateDialog();

    void setOperationUpdate(const QStringList &paths);
    void setOperationCheckout(const QSVNCheckoutDialog &dlg);

    enum QSVNOperationType {QSVNOperationNone, QSVNOperationCommit, QSVNOperationUpdate, QSVNOperationUpdateToRevision, QSVNOperationCheckout};

signals:
    void update(QStringList pathList, svn_opt_revision_t revision, svn_depth_t depth, bool depthIsSticky, bool ignoreExternals, bool allowUnverObstructions, bool addsAsModification, bool makeParents);
    void checkout(QString url, QString path, svn_opt_revision_t peg_revision, svn_opt_revision_t revision, svn_depth_t depth, bool ignore_externals, bool allow_unver_obstructions);

private slots:
    void workerStarted();
    void svnProgress(int progress, int total);
    void svnNotify(svn_wc_notify_t notify);
    void svnFinished(bool result);
    void svnError(QString text);

private:
    Ui::QSVNUpdateDialog *ui;
    QSVNThread m_thread;

    QStringList m_paths;
    QString m_url;
    QString m_path;

    svn_opt_revision_t m_revision;
    svn_opt_revision_t m_peg_revision;
    svn_depth_t m_depth;
    QSVNOperationType m_operation;
    svn_boolean_t m_ignore_externals;
    svn_boolean_t m_allow_unver_obstructions;
};

#endif // QSVNUPDATEDIALOG_H
