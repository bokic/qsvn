#ifndef QSVNUPDATETOREVISIONDIALOG_H
#define QSVNUPDATETOREVISIONDIALOG_H

#include <QDialog>

namespace Ui {
class QSVNUpdateToRevisionDialog;
}

class QSVNUpdateToRevisionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSVNUpdateToRevisionDialog(QWidget *parent = 0);
    ~QSVNUpdateToRevisionDialog();

private:
    Ui::QSVNUpdateToRevisionDialog *ui;
};

#endif // QSVNUPDATETOREVISIONDIALOG_H
