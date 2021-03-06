#ifndef QSVNCOMMITDIALOG_H
#define QSVNCOMMITDIALOG_H

#include "qsvn.h"
#include "qsvnthread.h"

#include <QStringList>
#include <QDialog>


namespace Ui {
class QSVNCommitDialog;
}

class QSVNCommitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSVNCommitDialog(const QStringList &items, QWidget *parent = 0);
    ~QSVNCommitDialog();

    QStringList ui_checked_path_items() const;
    svn_depth_t ui_depth() const;
    bool ui_keep_locks() const;
    bool ui_changelist() const;
    bool ui_m_commit_as_operations() const;
    QString ui_message() const;

signals:
    void status(QString path, svn_opt_revision_t revision, svn_depth_t depth, svn_boolean_t get_all, svn_boolean_t update, svn_boolean_t no_ignore, svn_boolean_t ignore_externals, svn_boolean_t depth_as_sticky);

private slots:
    void statusFinished(QList<QSvnStatusItem> items, QSvnError error);
    void on_showUnversioned_checkBox_stateChanged(int state);
    void on_showLog_pushButton_clicked();
    void filesChecked(int index, bool state);
    void label_clicked();
    void on_rescentMessage_toolButton_clicked();

private:
    void updateTotalAndChecked();
    void removeNormalSvnFiles(QList<QSvnStatusItem> &items);
    void updateLabelsState();
    virtual void accept() override;

    Ui::QSVNCommitDialog *ui;
    QSVNThread m_thread;

    QStringList m_items;
    QString m_commonDir;

    QSvn::QSVNOperationType m_operation;
};

#endif // QSVNCOMMITDIALOG_H
