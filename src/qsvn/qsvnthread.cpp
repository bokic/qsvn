#include "qsvnthread.h"
#include "QObject"

QSVNThread::QSVNThread(QObject *parent) :
    QThread(parent),
    m_worker(nullptr)
{
    m_startupMutex.lock();
}

void QSVNThread::run()
{
    m_worker = new QSvn();
    m_worker->moveToThread(currentThread());

    m_worker->init();

    m_startupMutex.unlock();

    exec();

    delete m_worker;
    m_worker = nullptr;
}

void QSVNThread::waitForStartup()
{
    m_startupMutex.lock();
    m_startupMutex.unlock();
}
