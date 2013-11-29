#ifndef QSVNTHREAD_H
#define QSVNTHREAD_H

#include "qsvn.h"

#include <QThread>


class QSVNThread : public QThread
{
    Q_OBJECT
public:
    explicit QSVNThread(QObject *parent = 0);

signals:

public slots:

protected:
    void run() override;

public:
    QSvn *m_worker;

};

#endif // QSVNTHREAD_H
