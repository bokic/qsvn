#include "qsvncommithistorydialog.h"
#include "ui_qsvncommithistorydialog.h"

QSvnCommitHistoryDialog::QSvnCommitHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSvnCommitHistoryDialog)
{
    ui->setupUi(this);
}

QSvnCommitHistoryDialog::~QSvnCommitHistoryDialog()
{
    delete ui;
}

void QSvnCommitHistoryDialog::setItems(const QStringList &items)
{
    m_items = items;

    ui->items_listWidget->clear();

    foreach(QString item, m_items)
    {
        int index;

        index = item.indexOf('\r');
        if (index >=0) item = item.left(index);

        index = item.indexOf('\n');
        if (index >=0) item = item.left(index);

        ui->items_listWidget->addItem(item);
    }
}

QString QSvnCommitHistoryDialog::selectedItem() const
{
    QString ret;

    if (ui->items_listWidget->currentItem())
    {
        ret = m_items.at(ui->items_listWidget->currentRow());
    }

    return ret;
}

void QSvnCommitHistoryDialog::on_items_listWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    done(QDialog::Accepted);
}
