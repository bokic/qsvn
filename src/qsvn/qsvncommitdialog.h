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
    void setOperationStatus(const QStringList &paths);

private slots:
    void workerStarted();

private:
    Ui::QSVNCommitDialog *ui;
    QSVNThread m_thread;

    QStringList m_paths;

    QSvn::QSVNOperationType m_operation;
};

#endif // QSVNCOMMITDIALOG_H
