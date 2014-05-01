#include "qsvnmessagelogdialog.h"
#include "ui_qsvnmessagelogdialog.h"

#include <QTableWidgetItem>


QSVNMessageLogDialog::QSVNMessageLogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNMessageLogDialog),
    m_thread(this),
    m_selectedRevision(-1)
{
    ui->setupUi(this);

    m_start.kind = svn_opt_revision_head;
    m_start.value.number = 0;
    m_end.kind = svn_opt_revision_date;
    m_end.value.date = QDate::currentDate().addMonths(-1).toJulianDay();
    m_peg.kind = svn_opt_revision_unspecified;
    m_peg.value.number = 0;

    ui->tableWidget_revisions->verticalHeader()->setDefaultSectionSize(ui->tableWidget_revisions->fontMetrics().height() + 4);
    ui->tableWidget_revisions->setColumnWidth(0, 70);
    ui->tableWidget_revisions->setColumnWidth(1, 80);
    ui->tableWidget_revisions->setColumnWidth(2, 120);
    ui->tableWidget_revisions->setColumnWidth(3, 180);
    ui->tableWidget_revisions->setColumnWidth(4, 380);

    const QDate &today = QDate::currentDate();
    const QDate &oneMonthFromToday = QDate::currentDate().addMonths(-1);


    ui->dateEdit_from->setDate(oneMonthFromToday);
    ui->dateEdit_to->setDate(today);

    ui->dateEdit_from->setMaximumDate(ui->dateEdit_to->date());
    ui->dateEdit_to->setMinimumDate(ui->dateEdit_from->date());

    m_thread.start();
    m_thread.waitForStartup();

    connect(this, &QSVNMessageLogDialog::messageLog, m_thread.m_worker, &QSvn::messageLog);
    connect(m_thread.m_worker, &QSvn::messageLogFinished, this, &QSVNMessageLogDialog::messageLogFinished);

    setCursor(Qt::BusyCursor);
}

QSVNMessageLogDialog::~QSVNMessageLogDialog()
{
    emit m_thread.quit();
    m_thread.wait();

    delete ui; ui = nullptr;
}

void QSVNMessageLogDialog::setUrlLocations(const QStringList &locations)
{
    m_locations = locations;

    emit messageLog(locations, m_start, m_end, m_peg);
}

int QSVNMessageLogDialog::selectedRevision()
{
    return m_selectedRevision;
}

void QSVNMessageLogDialog::messageLogFinished(QList<QMessageLogItem> items, QSvnError err)
{
    ui->tableWidget_revisions->setRowCount(items.count());

    for(int c = 0; c < items.count(); c++)
    {
        const QMessageLogItem &item = items.at(c);

        ui->tableWidget_revisions->setItem(c, 0, new QTableWidgetItem(QString::number(item.revision)));
        ui->tableWidget_revisions->setItem(c, 1, new QTableWidgetItem("")); // TODO: 2nd column implementation is missing(messageLogFinished).
        ui->tableWidget_revisions->setItem(c, 2, new QTableWidgetItem(item.author));
        ui->tableWidget_revisions->setItem(c, 3, new QTableWidgetItem(item.date.toString()));
        ui->tableWidget_revisions->setItem(c, 4, new QTableWidgetItem(item.message));
    }

    setCursor(Qt::ArrowCursor);
}

void QSVNMessageLogDialog::on_tableWidget_revisions_itemSelectionChanged()
{
    QString cellText;
    int row;

    row = ui->tableWidget_revisions->currentRow();
    cellText = ui->tableWidget_revisions->item(row, 0)->text();
    m_selectedRevision = cellText.toInt();

    ui->textEdit_revisionText->setText(ui->tableWidget_revisions->item(ui->tableWidget_revisions->currentRow(), 4)->text());
}

void QSVNMessageLogDialog::on_tableWidget_revisions_itemDoubleClicked(QTableWidgetItem *item)
{
    Q_UNUSED(item);

    accept();
}
