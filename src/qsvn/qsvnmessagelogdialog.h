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

    void setLocation(const QString &location);

private:
    Ui::QSVNMessageLogDialog *ui;

    QString m_location;
};

#endif // QSVNMESSAGELOGDIALOG_H
