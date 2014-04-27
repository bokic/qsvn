#ifndef QSVNMESSAGELOGDIALOG_H
#define QSVNMESSAGELOGDIALOG_H

#include "qsvnthread.h"

#include <QTableWidgetItem>
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

    void setUrlLocations(const QStringList &locations);
    int selectedRevision();

signals:
    void messageLog(const QStringList &locations);

private slots:
    void messageLogFinished(QList<QMessageLogItem> items);
    void on_tableWidget_revisions_itemSelectionChanged();
    void on_tableWidget_revisions_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::QSVNMessageLogDialog *ui;
    QSVNThread m_thread;
    QStringList m_locations;
    int m_selectedRevision;
};

#endif // QSVNMESSAGELOGDIALOG_H
