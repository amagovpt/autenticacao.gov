#ifndef __PORTABLE_SLEEP_H__
#define __PORTABLE_SLEEP_H__

#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif

class PortableSleep
{
    public:
        static void msleep(unsigned int milliSec)
        {
#ifdef _WIN32
            Sleep(milliSec);
#else
            usleep(milliSec * 1000);
#endif
        };
};

#endif // __PORTABLE_SLEEP_H__
