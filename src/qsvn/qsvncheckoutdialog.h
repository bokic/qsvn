#ifndef QSVNCHECKOUTDIALOG_H
#define QSVNCHECKOUTDIALOG_H

#include <QDialog>

namespace Ui {
class QSVNCheckoutDialog;
}

class QSVNCheckoutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSVNCheckoutDialog(QWidget *parent = 0);
    ~QSVNCheckoutDialog();

    void loadSettings();

private slots:
    void on_pushButton_URL_clicked();

private:
    Ui::QSVNCheckoutDialog *ui;
};

#endif // QSVNCHECKOUTDIALOG_H
