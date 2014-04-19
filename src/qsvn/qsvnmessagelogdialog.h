#ifndef QSVNMESSAGELOGDIALOG_H
#define QSVNMESSAGELOGDIALOG_H

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

private:
    Ui::QSVNMessageLogDialog *ui;
};

#endif // QSVNMESSAGELOGDIALOG_H
