/*-****************************************************************************

* Copyright (C) 2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
*
* Licensed under the EUPL V.1.1

****************************************************************************-*/

#include "concurrent.h"
#include <QMutableListIterator> 
#include "eidlib.h"

QList<QFuture<void>> Concurrent::m_futures;
QMutex Concurrent::m_cleaningListMutex;
bool Concurrent::m_cleaningFutureList = false;

void Concurrent::waitForAll()
{
    for (auto future : m_futures)
        future.waitForFinished();
}

bool Concurrent::waitForAll(int sec)
{
    int timeleft = sec;
    int timestep = 1; // Polling frequency
    while (timeleft > 0)
    {
        bool allFinished = true;
        for (size_t i = 0; i < m_futures.size(); i++)
        {
            if (!m_futures[i].isFinished()) {
                allFinished = false;
                break;
            }
        }

        if (allFinished)
            return true;

        QThread::sleep(timestep);
        timeleft -= timestep;
    }

    return false;
}

void Concurrent::cleanFutureList()
{
    // make sure only 1 thread is cleaning the list
    m_cleaningListMutex.lock();
    if (m_cleaningFutureList)
    {
        m_cleaningListMutex.unlock();
        return;
    }
    m_cleaningFutureList = true;
    m_cleaningListMutex.unlock();

    QMutableListIterator<QFuture<void>> it(m_futures);
    while (it.hasNext())
    {
        if (it.next().isFinished())
        {
            it.remove();
        }
    }

    m_cleaningFutureList = false;
}
