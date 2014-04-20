#include "qsvnmessagelogdialog.h"
#include "ui_qsvnmessagelogdialog.h"

#include <QTableWidgetItem>


QSVNMessageLogDialog::QSVNMessageLogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNMessageLogDialog),
    m_thread(this)
{
    ui->setupUi(this);

    m_thread.start();
    m_thread.waitForStartup();

    connect(this, &QSVNMessageLogDialog::messageLog, m_thread.m_worker, &QSvn::messageLog);
    connect(m_thread.m_worker, &QSvn::messageLogFinished, this, &QSVNMessageLogDialog::messageLogFinished);
}

QSVNMessageLogDialog::~QSVNMessageLogDialog()
{
    delete ui;
}

void QSVNMessageLogDialog::setLocation(const QString &location)
{
    m_location = location;

    emit messageLog(location);
}

void QSVNMessageLogDialog::messageLogFinished(QList<QMessageLogItem> items)
{
    ui->tableWidget_revisions->setRowCount(items.count());

    for(int c = 0; c < items.count(); c++)
    {
        const QMessageLogItem &item = items.at(c);

        ui->tableWidget_revisions->setItem(c, 0, new QTableWidgetItem(QString::number(item.revision)));
        ui->tableWidget_revisions->setItem(c, 2, new QTableWidgetItem(item.author));
        ui->tableWidget_revisions->setItem(c, 3, new QTableWidgetItem(item.date.toString()));
        ui->tableWidget_revisions->setItem(c, 4, new QTableWidgetItem(item.message));
    }

    return;
}
