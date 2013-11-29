#ifndef QSVNCOMMITDIALOG_H
#define QSVNCOMMITDIALOG_H

#include <QDialog>

namespace Ui {
class QSVNCommitDialog;
}

class QSVNCommitDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QSVNCommitDialog(QWidget *parent = 0);
    ~QSVNCommitDialog();
    
private:
    Ui::QSVNCommitDialog *ui;
};

#endif // QSVNCOMMITDIALOG_H
