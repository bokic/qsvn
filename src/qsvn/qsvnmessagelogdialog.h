#ifndef QSVNMESSAGELOGDIALOG_H
#define QSVNMESSAGELOGDIALOG_H

#include "qsvnthread.h"

#include <QDialog>

namespace Ui {
class QSVNMessageLogDialog;
}

class QSVNMessageLogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSVNMessageLogDialog(QWidget *parent = 0);
    ~QSVNMessageLogDialog();

    void setLocation(const QString &location);

signals:
    void messageLog(const QString &location);

public slots:
    void messageLogFinished(QList<QMessageLogItem> items);

private:
    Ui::QSVNMessageLogDialog *ui;
    QSVNThread m_thread;
    QString m_location;
};

#endif // QSVNMESSAGELOGDIALOG_H
