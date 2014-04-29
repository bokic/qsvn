#ifndef QSVNCOMMITHISTORYDIALOG_H
#define QSVNCOMMITHISTORYDIALOG_H

#include <QDialog>

namespace Ui {
class QSvnCommitHistoryDialog;
}

class QSvnCommitHistoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSvnCommitHistoryDialog(QWidget *parent = 0);
    ~QSvnCommitHistoryDialog();

    void setItems(const QStringList &items);
    QString selectedItem() const;

private slots:
    void on_items_listWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::QSvnCommitHistoryDialog *ui;

    QStringList m_items;
};

#endif // QSVNCOMMITHISTORYDIALOG_H
