/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#include "UnitTest++/src/UnitTest++.h"
#include "../common/Thread.h"

#include <sys/timeb.h>

using namespace eIDMW;

class TestThread: public CThread
{
public:
	TestThread(int delayMillis)
	{
		m_iDelayMillis = delayMillis;
		m_bHasRun = false;
	}

	void Run()
	{
		CThread::SleepMillisecs(m_iDelayMillis);
		m_bHasRun = true;
	}

	bool m_bHasRun;

private:
	int m_iDelayMillis;
};

TEST(Run)
{

	int runTime1 = 200;			// Should be longer than the others !
	int runTime2 = 100;
	int runTime3 = 100;
	int totalRunTime = runTime1 + runTime2 + runTime3;

	TestThread tt1(runTime1);
	TestThread tt2(runTime2);
	TestThread tt3(runTime3);

	timeb pTime;
	ftime(&pTime);

	double time1 = pTime.time + (double) pTime.millitm/1000.;

	int res1 = tt1.Start();
	int res2 = tt2.Start();
	int res3 = tt3.Start();
	CHECK_EQUAL(0, res1);
	CHECK_EQUAL(0, res2);
	CHECK_EQUAL(0, res3);

	// Wait till all threads have finished
	while (tt1.IsRunning() || tt2.IsRunning() || tt3.IsRunning()) {}

	// Check if all threads have run
	CHECK_EQUAL(true, tt1.m_bHasRun);
	CHECK_EQUAL(true, tt2.m_bHasRun);
	CHECK_EQUAL(true, tt3.m_bHasRun);

	ftime(&pTime);
	double time2 = pTime.time + (double) pTime.millitm/1000.;

	// The time needed to run all threads should be at least 100 msec
	// and should normally be less then the sum of all 3 thread times together
	double time = (time2 - time1)*1000;

	// Time to run the thread must be >= time to run the longest thread
	CHECK_EQUAL(true, time >= runTime1);

	// Time to run must be < sum of all thread times
	double maxTime = totalRunTime;
	if (time <= maxTime) CHECK_EQUAL(true, true);
	else  CHECK_EQUAL(maxTime, time);
	
}
