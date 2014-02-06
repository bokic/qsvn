#ifndef QSVNCOMMITDIALOG_H
#define QSVNCOMMITDIALOG_H

#include "qsvn.h"
#include "qsvnthread.h"

#include <QDialog>


namespace Ui {
class QSVNCommitDialog;
}

class QSVNCommitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSVNCommitDialog(QWidget *parent = 0);
    ~QSVNCommitDialog();
    void setOperationStatus(const QString &path);

signals:
    void status(QString path, svn_opt_revision_t revision, svn_depth_t depth, svn_boolean_t get_all, svn_boolean_t update, svn_boolean_t no_ignore, svn_boolean_t ignore_externals, svn_boolean_t depth_as_sticky);

private slots:
    void statusFinished(QList<QSvnStatusItem> items, bool error);
private:
    Ui::QSVNCommitDialog *ui;
    QSVNThread m_thread;

    QStringList m_paths;

    QSvn::QSVNOperationType m_operation;
};

#endif // QSVNCOMMITDIALOG_H
