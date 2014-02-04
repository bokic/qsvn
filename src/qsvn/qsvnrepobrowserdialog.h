#ifndef QSVNREPOBROWSERDIALOG_H
#define QSVNREPOBROWSERDIALOG_H

#include "qsvnthread.h"

#include <QDialog>

namespace Ui {
class QSVNRepoBrowserDialog;
}

class QSVNRepoBrowserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSVNRepoBrowserDialog(QWidget *parent = 0);
    ~QSVNRepoBrowserDialog();

    void setURL(const QString &url);
    QString URL();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void workerFinished();
    void workerResult(QRepoBrowserResult items);
    void workerError(QString text);
    void on_comboBox_URL_push(const QString &text);
    void on_pushButton_Head_clicked();
    void on_treeWidget_Files_itemSelectionChanged();

signals:
    void getURL(QString url, svn_opt_revision_t revision, bool recursion);

private:
    void getUrlItems();
    void loadSettings();
    void updateSettings();

    Ui::QSVNRepoBrowserDialog *ui;
    QSVNThread m_thread;
    QHash<QString, QRepoBrowserResult> m_cachedFolders;
};

#endif // QSVNREPOBROWSERDIALOG_H
