/*-****************************************************************************

* Copyright (C) 2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
*
* Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef CONCURRENT_H
#define CONCURRENT_H

#include <QtConcurrent>
#include <atomic>
#include <tuple>

class Concurrent {
public:
	/* Arguments are copied and the copies are passed to the function, as QtConcurrent::run did in Qt 5.
	   Qt 6 no longer binds stored arguments to non-const reference parameters, hence the lambdas. */
	template <typename T, typename... Params, typename... Args>
	static QFuture<T> run(T (*functionPointer)(Params...), const Args &...args) {
		return track(QtConcurrent::run([functionPointer, copies = std::make_tuple(args...)]() mutable {
			return std::apply([functionPointer](auto &...a) { return functionPointer(a...); }, copies);
		}));
	}

	template <typename T, typename Class, typename... Params, typename... Args>
	static QFuture<T> run(Class *object, T (Class::*fn)(Params...), const Args &...args) {
		return track(QtConcurrent::run([object, fn, copies = std::make_tuple(args...)]() mutable {
			return std::apply([object, fn](auto &...a) { return (object->*fn)(a...); }, copies);
		}));
	}

	/* Wait for launched threads to finish */
	static void waitForAll();

	/* Wait for threads for at most sec seconds. Returns false if timeout, true otherwise. */
	static bool waitForAll(int sec);

private:
	template <typename T> static QFuture<T> track(QFuture<T> future) {
		cleanFutureList();
		m_futures.append(QFuture<void>(future));
		return future;
	}

	static void cleanFutureList();

	static QList<QFuture<void>> m_futures;

	static QMutex m_cleaningListMutex;
	static bool m_cleaningFutureList;
};

#endif
