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
    void on_radioButton_head_clicked();
    void on_radioButton_revision_clicked();
    void on_pushButton_revision_clicked();
    void on_lineEdit_dir_textChanged(const QString &text);
    void on_comboBox_URL_currentTextChanged(const QString &text);

private:
    Ui::QSVNCheckoutDialog *ui;
    QString m_TargetDir;
};

#endif // QSVNCHECKOUTDIALOG_H
