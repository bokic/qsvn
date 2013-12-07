#include "qsvncheckoutdialog.h"
#include "qsvncommitdialog.h"
#include "qsvnupdatedialog.h"
#include "qsvnrepobrowserdialog.h"
#include "qsvn.h"

#include <QApplication>

#include <stdio.h>

bool setAppSettings()
{
    QCoreApplication::setOrganizationName("Boris Barbulovski");
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
    qRegisterMetaType<svn_depth_t>("svn_depth_t");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage qsvn <checkout, commit, update, browse>\n");

        return -1;
    }

    registerClasses();

    if (strcmp(argv[1], "checkout") == 0)
    {
        QApplication a(argc, argv);
        if (!setAppSettings()) return -1;
        QSVNCheckoutDialog w;
        QSVNUpdateDialog ud;

        if (argc > 2)
        {
            w.setTargetDir(QString::fromUtf8(argv[2]));
        }

        if (w.exec() == QDialog::Accepted)
        {
            ud.setOperationCheckout(w);
            ud.show();
        }

        return a.exec();
    }
    else if (strcmp(argv[1], "commit") == 0)
    {
        QApplication a(argc, argv);
        if (!setAppSettings()) return -1;
        QSVNCommitDialog w;
        w.show();

        return a.exec();
    }
    else if (strcmp(argv[1], "update") == 0)
    {
        QApplication a(argc, argv);
        if (!setAppSettings()) return -1;

        QStringList urls;

        for(int c = 2; c < argc; c++)
        {
            urls.append(QString::fromUtf8(argv[c]));
        }

        QSVNUpdateDialog w;
        w.setOperationUpdate(urls);
        w.show();

        return a.exec();
    }
    else if (strcmp(argv[1], "update-to-revision") == 0)
    {
        QApplication a(argc, argv);
        if (!setAppSettings()) return -1;

        QStringList urls;

        for(int c = 2; c < argc; c++)
        {
            urls.append(QString::fromUtf8(argv[c]));
        }

        QSVNUpdateDialog w;
        w.setOperationUpdate(urls);
        w.show();

        return a.exec();
    }
    else if (strcmp(argv[1], "browse") == 0)
    {
        QApplication a(argc, argv);
        if (!setAppSettings()) return -1;
        QSVNRepoBrowserDialog w;
        w.show();

        return a.exec();
    }

    printf("Unknown usage.\nUsage qsvn <commit, update, browse>\n");

    return -1;
}
