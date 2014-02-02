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
class QSVNUpdateToRevisionDialog;

class QSVNUpdateDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QSVNUpdateDialog(QWidget *parent = 0);
    ~QSVNUpdateDialog();

    void setOperationUpdate(const QStringList &paths);
    void setOperationUpdateToRevision(const QSVNUpdateToRevisionDialog &dlg);
    void setOperationCheckout(const QSVNCheckoutDialog &dlg);

signals:
    void update(QStringList pathList, svn_opt_revision_t revision, svn_depth_t depth, bool depthIsSticky, bool ignoreExternals, bool allowUnverObstructions, bool addsAsModification, bool makeParents);
    void checkout(QString url, QString path, svn_opt_revision_t peg_revision, svn_opt_revision_t revision, svn_depth_t depth, bool ignore_externals, bool allow_unver_obstructions);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void workerStarted();
    void workerFinished();
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
    bool m_depth_is_sticky;
    bool m_ignore_externals;
    bool m_allow_unver_obstructions;
    bool m_add_as_modification;
    bool m_make_parents;
};

#endif // QSVNUPDATEDIALOG_H
