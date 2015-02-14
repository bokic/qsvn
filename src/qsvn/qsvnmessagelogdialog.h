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
    void messageLog(QStringList locations, svn_opt_revision_t start, svn_opt_revision_t end, svn_opt_revision_t peg);

private slots:
    void messageLogFinished(QList<QMessageLogItem> items, QSvnError err);
    void on_tableWidget_revisions_itemSelectionChanged();
    void on_tableWidget_revisions_itemDoubleClicked(QTableWidgetItem *item);

private:
    void svnThreadIsWorking(bool working);
    Ui::QSVNMessageLogDialog *ui;
    QSVNThread m_thread;
    QStringList m_locations;
    svn_opt_revision_t m_start;
    svn_opt_revision_t m_end;
    svn_opt_revision_t m_peg;
    int m_selectedRevision;
};

#endif // QSVNMESSAGELOGDIALOG_H
