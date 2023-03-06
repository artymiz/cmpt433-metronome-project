#include <time.h>
#include <stdlib.h>

void Timing_waitMs(long long ms)
{
    const long long NS_PER_MS = 1000000;
    const long long NS_PER_SECOND = 1000000000;
    long long ns = ms * NS_PER_MS;
    struct timespec ts = {ns / NS_PER_SECOND, ns % NS_PER_SECOND};
    nanosleep(&ts, (struct timespec *) NULL);
}

long long Timing_stampMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long ns = spec.tv_nsec;
    long long ms = seconds * 1000 + ns / 1000000;
    return ms;
}
