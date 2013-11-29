#include "qsvnrepobrowserdialog.h"
#include "ui_qsvnrepobrowserdialog.h"
#include "qsvnselectrevisiondialog.h"
#include "qsvn.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>


QSVNRepoBrowserDialog::QSVNRepoBrowserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSVNRepoBrowserDialog),
    m_thread(this)
{
    ui->setupUi(this);

    loadSettings();

    ui->comboBox_URL->setCurrentIndex(-1);

    connect(&m_thread, &QThread::started, this, &QSVNRepoBrowserDialog::workerStarted);
    connect(&m_thread, &QThread::finished, this, &QSVNRepoBrowserDialog::workerFinished);

    m_thread.start();
}

QSVNRepoBrowserDialog::~QSVNRepoBrowserDialog()
{
    if (m_thread.isRunning())
    {
        m_thread.exit();

        m_thread.wait();
    }

    delete ui;
}

void QSVNRepoBrowserDialog::workerStarted()
{
    connect(m_thread.m_worker, &QSvn::repoBrowserResult, this, &QSVNRepoBrowserDialog::workerResult);
    connect(m_thread.m_worker, &QSvn::error, this, &QSVNRepoBrowserDialog::workerError);
    connect(this, &QSVNRepoBrowserDialog::getURL, m_thread.m_worker, &QSvn::repoBrowser);
}

void QSVNRepoBrowserDialog::workerFinished()
{
    close();
}

void QSVNRepoBrowserDialog::workerResult(QRepoBrowserResult items)
{
    setCursor(Qt::ArrowCursor);

    ui->comboBox_URL->setEnabled(true);
    ui->pushButton_Head->setEnabled(true);
    ui->treeWidget_Files->setEnabled(true);
    ui->tableWidget_Files->setEnabled(true);
    ui->pushButton_Ok->setEnabled(true);

    if (!items.error.isEmpty())
    {
        QMessageBox::critical(this, tr("SVN error"), tr("Error: %1").arg(items.error));

        return;
    }

    updateSettings();

    QTreeWidgetItem *top;

    if (!ui->treeWidget_Files->currentItem())
    {
        top = new QTreeWidgetItem(QStringList() << ui->comboBox_URL->currentText());
        top->setIcon(0, QIcon(":/icons/folder"));

        ui->treeWidget_Files->clear();
        ui->treeWidget_Files->insertTopLevelItem(0, top);
    }
    else
    {
        top = ui->treeWidget_Files->currentItem();
    }

    // Clear tree child items
    bool addTreeChildren = (top->childCount() == 0);

    // Clear grid items
    ui->tableWidget_Files->setRowCount(0);

    // Add grid dirs
    for(int c = 0; c < items.files.count(); c++)
    {
        const QRepoBrowserFile &item = items.files.at(c);
        QTreeWidgetItem *treeChild;

        if (!item.isdir)
        {
            continue;
        }

        // Add tree item
        if (addTreeChildren)
        {
            treeChild = new QTreeWidgetItem(QStringList() << item.filename);

            treeChild->setIcon(0, QIcon(":/icons/folder"));

            top->addChild(treeChild);
        }

        // Add grid folders row
        int new_row = ui->tableWidget_Files->rowCount();
        ui->tableWidget_Files->setRowCount(new_row + 1);

        ui->tableWidget_Files->setItem(new_row, 0, new QTableWidgetItem(QIcon(":/icons/folder"), item.filename));

        ui->tableWidget_Files->setItem(new_row, 2, new QTableWidgetItem(QString::number(item.revision)));

        ui->tableWidget_Files->setItem(new_row, 3, new QTableWidgetItem(item.author));

        ui->tableWidget_Files->setItem(new_row, 5, new QTableWidgetItem(item.modified.toString()));
    }

    ui->treeWidget_Files->expandItem(top);

    // Add grid files
    for(int c = 0; c < items.files.count(); c++)
    {
        const QRepoBrowserFile &item = items.files.at(c);
        QString file_ext;
        QString filename;

        if (item.isdir)
        {
            continue;
        }

        // Add grid folders row
        int new_row = ui->tableWidget_Files->rowCount();
        ui->tableWidget_Files->setRowCount(new_row + 1);

        filename = item.filename;

        ui->tableWidget_Files->setItem(new_row, 0, new QTableWidgetItem(QIcon(":/icons/file"), filename));

        if (filename.contains("."))
        {
            file_ext = filename.mid(filename.lastIndexOf("."));
        }

        ui->tableWidget_Files->setItem(new_row, 1, new QTableWidgetItem(file_ext));

        ui->tableWidget_Files->setItem(new_row, 2, new QTableWidgetItem(QString::number(item.revision)));

        ui->tableWidget_Files->setItem(new_row, 3, new QTableWidgetItem(item.author));

        ui->tableWidget_Files->setItem(new_row, 4, new QTableWidgetItem(QString::number(item.size)));

        ui->tableWidget_Files->setItem(new_row, 5, new QTableWidgetItem(item.modified.toString()));
    }

    QStringList urlSegments;

    while(top)
    {
        urlSegments.prepend(top->text(0));

        top = top->parent();
    }

    QString url = urlSegments.join("/");

    if (!m_cachedFolders.contains(url))
    {
        m_cachedFolders[url] = items;
    }
}

void QSVNRepoBrowserDialog::workerError(QString text)
{
    QMessageBox::critical(this, tr("SVN error"), tr("Error: %1").arg(text));
}

void QSVNRepoBrowserDialog::closeEvent(QCloseEvent *event)
{
    if (m_thread.isRunning())
    {
        m_thread.exit();

        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void QSVNRepoBrowserDialog::on_comboBox_URL_push(const QString &text)
{
    Q_UNUSED(text);

    ui->pushButton_Head->setText(tr("HEAD"));

    ui->treeWidget_Files->clear();
    ui->tableWidget_Files->setRowCount(0);

    getUrlItems();
}

void QSVNRepoBrowserDialog::on_pushButton_Head_clicked()
{
    QSVNSelectRevisionDialog dlg(this, ui->pushButton_Head->text());

    if (dlg.exec() == QDialog::Accepted)
    {
        int rev;

        rev = dlg.revision();

        if (rev > 0)
        {
            QString newText = QString::number(rev);

            if (ui->pushButton_Head->text() == newText)
            {
                return;
            }

            ui->pushButton_Head->setText(newText);
        }
        else
        {
            QString newText = tr("HEAD");

            if (ui->pushButton_Head->text() == newText)
            {
                return;
            }

            ui->pushButton_Head->setText(newText);
        }

        m_cachedFolders.clear();

        getUrlItems();
    }
}

void QSVNRepoBrowserDialog::on_treeWidget_Files_itemSelectionChanged()
{
    getUrlItems();
}

void QSVNRepoBrowserDialog::getUrlItems()
{
    QString url;
    int revInt;

    ui->comboBox_URL->setEnabled(false);
    ui->pushButton_Head->setEnabled(false);
    ui->treeWidget_Files->setEnabled(false);
    ui->tableWidget_Files->setEnabled(false);
    ui->pushButton_Ok->setEnabled(false);

    revInt = ui->pushButton_Head->text().toInt();

    QTreeWidgetItem *item = ui->treeWidget_Files->currentItem();

    if (item == nullptr)
    {
        url = ui->comboBox_URL->currentText();
    }
    else
    {
        QStringList items;

        url = item->text(0);

        while(item)
        {
            items.prepend(item->text(0));

            item = item->parent();
        }

        url = items.join("/");
    }

    if (m_cachedFolders.contains(url))
    {
        workerResult(m_cachedFolders[url]);
    }
    else
    {
        svn_opt_revision_t rev;

        if (revInt <= 0)
        {
            rev.kind = svn_opt_revision_head;
        }
        else
        {
            rev.kind = svn_opt_revision_number;
            rev.value.number = revInt;
        }

        emit getURL(url, rev, false);

        setCursor(Qt::BusyCursor);
    }
}

void QSVNRepoBrowserDialog::loadSettings()
{
    QSettings settings;

    settings.beginGroup("URL");

    for (int c = 1; settings.contains(QString("URL_%1").arg(c)); c++)
    {
        ui->comboBox_URL->addItem(settings.value(QString("URL_%1").arg(c)).toString());
    }

    settings.endGroup();
}

void QSVNRepoBrowserDialog::updateSettings()
{
    QSettings settings;

    settings.beginGroup("URL");

    for(int c = 0; c < ui->comboBox_URL->count(); c++)
    {
        QString key = QString("URL_%1").arg(c + 1);
        QString value = ui->comboBox_URL->itemText(c);

        settings.setValue(key, value);
    }

    settings.endGroup();
}
