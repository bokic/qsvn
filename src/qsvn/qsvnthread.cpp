#include "qsvnthread.h"
#include "QObject"

QSVNThread::QSVNThread(QObject *parent) :
    QThread(parent),
    m_worker(nullptr)
{
}

void QSVNThread::run()
{
    m_worker = new QSvn();
    m_worker->moveToThread(currentThread());

    m_worker->init();


    exec();

    delete m_worker;
    m_worker = nullptr;
}
