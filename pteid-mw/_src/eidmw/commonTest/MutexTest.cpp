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
#include "../common/Mutex.h"

#include <sys/timeb.h>


#include <iostream>

#ifndef WIN32
#include "errno.h"
#endif

using namespace eIDMW;

bool bInCS = false;
bool bCsBroken = false;
CMutex mutex;

class TestMThread: public CThread
{
public:
	TestMThread(int delayMillis)
	{
		m_iDelayMillis = delayMillis;

	}

	void Run()
	{
	  mutex.Lock() ; // start Critical Section (SC)
	  if (bInCS)
	    bCsBroken = true;
	  bInCS = true;
	  SleepMillisecs(m_iDelayMillis);
	  bInCS = false;
	  mutex.Unlock(); // end CS
	}

private:
	int m_iDelayMillis;
};

TEST(Mutex)
{
	int errors = 0;

	bInCS = false;
	bCsBroken = false;

	// delays in milliseconds
	int delay1 = 150;
	int delay2 = 100;
	int delay3 = 150;
	int delay4 = 250;
	int delay5 = 500;
	int delay6 = 100;

	TestMThread tt1(delay1);
	TestMThread tt2(delay2);
	TestMThread tt3(delay3);
	TestMThread tt4(delay4);
	TestMThread tt5(delay5);
	TestMThread tt6(delay6);

	// use ftime() instead of time() because it has a granularity of millisecs,
	// while time() only measures seconds.
	timeb pTime1;
	ftime(&pTime1);

	tt1.Start();
	tt2.Start();
	tt3.Start();
	tt4.Start();
	tt5.Start();
	tt6.Start();

	while (tt1.IsRunning() || tt2.IsRunning() || tt3.IsRunning() || tt4.IsRunning() || tt5.IsRunning() || tt6.IsRunning());

	timeb pTime2;
	ftime(&pTime2);

	// Check if that no multiple threads were simultaniously in the critical section
	CHECK_EQUAL(false, bCsBroken);

	// difference in seconds with granularity of milliseconds
	int diff = (int) ((pTime2.time - pTime1.time) * 1000 + pTime2.millitm - pTime1.millitm);

	// If that the time to run all the threads is at leat the sum of the thread times
	//printf("diff = %d, delay = %d\n", diff, (delay1+delay2+delay3));
	CHECK_EQUAL(true, diff >= delay1+delay2+delay3+delay4+delay5+delay6);

}

TEST(Mutex2)
{
	int errors = 0;

	bInCS = false;
	bCsBroken = false;

	// delays in milliseconds
	int delay1 = 1;
	int delay2 = 2;
	int delay3 = 1;
	int delay4 = 1;
	int delay5 = 3;
	int delay6 = 1;

	TestMThread tt1(delay1);
	TestMThread tt2(delay2);
	TestMThread tt3(delay3);
	TestMThread tt4(delay4);
	TestMThread tt5(delay5);
	TestMThread tt6(delay6);

	// use ftime() instead of time() because it has a granularity of millisecs,
	// while time() only measures seconds.
	timeb pTime1;
	ftime(&pTime1);

	tt1.Start();
	tt2.Start();
	tt3.Start();
	tt4.Start();
	tt5.Start();
	tt6.Start();

	while (tt1.IsRunning() || tt2.IsRunning() || tt3.IsRunning() || tt4.IsRunning() || tt5.IsRunning() || tt6.IsRunning());

	timeb pTime2;
	ftime(&pTime2);

	// Check if that no multiple threads were simultaniously in the critical section
	CHECK_EQUAL(false, bCsBroken);

	// difference in seconds with granularity of milliseconds
	int diff = (int) ((pTime2.time - pTime1.time) * 1000 + pTime2.millitm - pTime1.millitm);

	// If that the time to run all the threads is at leat the sum of the thread times
	//printf("diff = %d, delay = %d\n", diff, (delay1+delay2+delay3));
	CHECK_EQUAL(true, diff >= delay1+delay2+delay3+delay4+delay5+delay6);

}