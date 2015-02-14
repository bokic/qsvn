#ifndef QSVNCLEANUPDIALOG_H
#define QSVNCLEANUPDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class QSvnCleanupDialog;
}

class QSvnCleanupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSvnCleanupDialog(const QStringList &paths, QWidget *parent = 0);
    ~QSvnCleanupDialog();

private:
    virtual void accept() override;

private:
    Ui::QSvnCleanupDialog *ui;
    QStringList m_paths;
};

#endif // QSVNCLEANUPDIALOG_H
