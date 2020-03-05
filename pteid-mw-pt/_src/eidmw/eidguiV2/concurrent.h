/*-****************************************************************************

* Copyright (C) 2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
*
* Licensed under the EUPL V.1.1

****************************************************************************-*/

#ifndef CONCURRENT_H
#define CONCURRENT_H

#include <QtConcurrent>
#include <atomic>

class Concurrent {
public:
    template <typename T, typename Class>
    static QFuture<T> run(Class *object, T(Class::*fn)())
    {
        QFuture<T> future = QtConcurrent::run(object, fn);
        cleanFutureList();
        m_futures.append(future);
        return future;
    }

    template <typename T, typename Class, typename Param1, typename Arg1>
    static QFuture<T> run(Class *object, T(Class::*fn)(Param1), const Arg1 &arg1)
    {
        QFuture<T> future = QtConcurrent::run(object, fn, arg1);
        cleanFutureList();
        m_futures.append(future);
        return future;
    }

    template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
    static QFuture<T> run(Class *object, T(Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2)
    {
        QFuture<T> future = QtConcurrent::run(object, fn, arg1, arg2);
        cleanFutureList();
        m_futures.append(future);
        return future;
    }

    template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
    static QFuture<T> run(Class *object, T(Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
    {
        QFuture<T> future = QtConcurrent::run(object, fn, arg1, arg2, arg3);
        cleanFutureList();
        m_futures.append(future);
        return future;
    }

    template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
    static QFuture<T> run(Class *object, T(Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
    {
        QFuture<T> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4);
        cleanFutureList();
        m_futures.append(future);
        return future;
    }

    template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
    static QFuture<T> run(Class *object, T(Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
    {
        QFuture<T> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4, arg5);
        cleanFutureList();
        m_futures.append(future);
        return future;
    }

    /* Wait for launched threads to finish */
    static void waitForAll();

    /* Wait for threads and terminate after sec seconds */
    static void waitForAllAndTerminate(int sec = 10);
private:
    static void cleanFutureList();

    static QList<QFuture<void>> m_futures;

    static QMutex m_cleaningListMutex;
    static bool m_cleaningFutureList;
};

#endif
