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

void Concurrent::waitForAllAndTerminate(int sec)
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
            return;

        QThread::sleep(timestep);
        timeleft -= timestep;
    }

    // Threads did not finished and timeout occured: exit process
    PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_CRITICAL, "eidgui", "Exiting application after timeout. Some threads may not have been correctly finished.");
    exit(0);
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
