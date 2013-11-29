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

private:
    Ui::QSVNSelectRevisionDialog *ui;
};

#endif // QSVNSELECTREVISIONDIALOG_H
