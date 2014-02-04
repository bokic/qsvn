#ifndef QSVNTHREAD_H
#define QSVNTHREAD_H

#include "qsvn.h"

#include <QThread>
#include <QMutex>


class QSVNThread : public QThread
{
    Q_OBJECT
public:
    explicit QSVNThread(QObject *parent = 0);

    void waitForStartup();

signals:

public slots:

protected:
    void run() override;

public:
    QSvn *m_worker;

private:
    QMutex m_startupMutex;

};

#endif // QSVNTHREAD_H
