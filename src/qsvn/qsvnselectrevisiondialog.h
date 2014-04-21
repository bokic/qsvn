#ifndef QSVNSELECTREVISIONDIALOG_H
#define QSVNSELECTREVISIONDIALOG_H

#include <QDialog>

namespace Ui {
class QSVNSelectRevisionDialog;
}

class QSVNSelectRevisionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSVNSelectRevisionDialog(QWidget *parent = 0, const QString &text = "");
    ~QSVNSelectRevisionDialog();
    int revision();

private slots:
    void on_lineEdit_Revision_editingFinished();

    void on_pushButton_ShowLog_clicked();

private:
    Ui::QSVNSelectRevisionDialog *ui;
    QStringList m_locations;
};

#endif // QSVNSELECTREVISIONDIALOG_H
