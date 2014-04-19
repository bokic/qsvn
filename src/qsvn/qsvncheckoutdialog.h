#ifndef QSVNCHECKOUTDIALOG_H
#define QSVNCHECKOUTDIALOG_H

#include <QDialog>

#include <svn_opt.h>


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
    void setTargetDir(const QString &dir);

    QString ui_url() const;
    QString ui_path() const;
    svn_opt_revision_t ui_revision() const;
    svn_depth_t ui_depth() const;
    svn_boolean_t ui_include_externals() const;
    svn_boolean_t ui_allow_unver() const;

private slots:
    void on_pushButton_URL_clicked();
    void on_pushButton_dir_clicked();

private:
    Ui::QSVNCheckoutDialog *ui;
};

#endif // QSVNCHECKOUTDIALOG_H
