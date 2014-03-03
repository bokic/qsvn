#ifndef QSVNLOGINDIALOG_H
#define QSVNLOGINDIALOG_H

#include <QDialog>

namespace Ui {
class QSVNLoginDialog;
}

class QSVNLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSVNLoginDialog(QWidget *parent = 0);
    ~QSVNLoginDialog();

private:
    Ui::QSVNLoginDialog *ui;
};

#endif // QSVNLOGINDIALOG_H
