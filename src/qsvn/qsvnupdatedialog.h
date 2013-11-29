#ifndef QSVNUPDATEDIALOG_H
#define QSVNUPDATEDIALOG_H

#include "qsvn.h"
#include "qsvnthread.h"

#include <QStringList>
#include <QDialog>

namespace Ui {
class QSVNUpdateDialog;
}

class QSVNUpdateDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QSVNUpdateDialog(QStringList paths, QWidget *parent = 0);
    ~QSVNUpdateDialog();

signals:
    void update(QStringList pathList, svn_opt_revision_t revision, svn_depth_t depth, bool depthIsSticky, bool ignoreExternals, bool allowUnverObstructions, bool addsAsModification, bool makeParents);

private slots:
    void workerStarted();
    void workerFinished();

private:
    Ui::QSVNUpdateDialog *ui;
    QSVNThread m_thread;

    QStringList m_paths;
    svn_opt_revision_t m_revision;
    svn_depth_t m_depth;

};

#endif // QSVNUPDATEDIALOG_H
