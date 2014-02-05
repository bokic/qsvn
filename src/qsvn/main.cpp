#include "qsvncheckoutdialog.h"
#include "qsvncommitdialog.h"
#include "qsvnupdatedialog.h"
#include "qsvnrepobrowserdialog.h"
#include "qsvnupdatetorevisiondialog.h"
#include "qsvn.h"

#include <QApplication>

#include <stdio.h>

bool setAppSettings()
{
    QCoreApplication::setOrganizationName("qsvn");
    QCoreApplication::setOrganizationDomain("bokicsoft.com");
    QCoreApplication::setApplicationName("qsvn");

    if (svn_cmdline_init("qsvn", stderr) != EXIT_SUCCESS)
    {
        return false;
    }

    return true;
}

void registerClasses()
{
    qRegisterMetaType<QRepoBrowserResult>("QRepoBrowserResult");
    qRegisterMetaType<svn_opt_revision_t>("svn_opt_revision_t");
    qRegisterMetaType<svn_wc_notify_t>("svn_wc_notify_t");
    qRegisterMetaType<svn_boolean_t>("svn_boolean_t");
    qRegisterMetaType<svn_depth_t>("svn_depth_t");
}

int main(int argc, char *argv[])
{
    QDialog *dlg;

    if (argc < 2)
    {
        printf("Usage qsvn <browse, checkout, commit, update, update-to-revision>\n");

        return -1;
    }

    registerClasses();

    QApplication a(argc, argv);
    if (!setAppSettings()) return -1;

    if (strcmp(argv[1], "browse") == 0)
    {
        dlg = new QSVNRepoBrowserDialog();
    }
    else if (strcmp(argv[1], "checkout") == 0)
    {
        QSVNCheckoutDialog w;

        if (argc > 2)
        {
            w.setTargetDir(QString::fromUtf8(argv[2]));
        }

        if (w.exec() == QDialog::Accepted)
        {
            dlg = new QSVNUpdateDialog();
            ((QSVNUpdateDialog *)dlg)->setOperationCheckout(w);
        }
        else
        {
            return 0;
        }
    }
    else if (strcmp(argv[1], "commit") == 0)
    {
        QStringList paths;

        for(int c = 2; c < argc; c++)
        {
            paths.append(QString::fromUtf8(argv[c]));
        }

        dlg = new QSVNCommitDialog();
        ((QSVNCommitDialog *)dlg)->setOperationStatus(paths);
    }
    else if (strcmp(argv[1], "update") == 0)
    {
        QStringList paths;

        for(int c = 2; c < argc; c++)
        {
            paths.append(QString::fromUtf8(argv[c]));
        }

        dlg = new QSVNUpdateDialog();
        ((QSVNUpdateDialog *)dlg)->setOperationUpdate(paths);
    }
    else if (strcmp(argv[1], "update-to-revision") == 0)
    {
        QStringList paths;

        for(int c = 2; c < argc; c++)
        {
            paths.append(QString::fromUtf8(argv[c]));
        }

        QSVNUpdateToRevisionDialog w(paths);

        if (w.exec() == QDialog::Accepted)
        {
            dlg = new QSVNUpdateDialog();
            ((QSVNUpdateDialog *)dlg)->setOperationUpdateToRevision(w);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        printf("Unknown usage.\nUsage qsvn <browse, checkout, commit, update, update-to-revision>\n");

        return -1;
    }

    dlg->show();

    return a.exec();
}
