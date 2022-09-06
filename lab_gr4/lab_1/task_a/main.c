#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct timespec timespec_normalized(time_t sec, long nsec)
{
	while(nsec >= 1000000000)
	{
		nsec -= 1000000000; // 9 zeros :)
		++sec;
	}
	while(nsec < 0)
	{
		nsec =+ 1000000000;
		--sec;
	}
	return (struct timespec){sec, nsec};
}

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs)
{
	return timespec_normalized(lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs)
{
	return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}

int timespec_cmp(struct timespec lhs, struct timespec rhs)
{
	if (lhs.tv_sec < rhs.tv_sec)
		return -1;
	if (lhs.tv_sec > rhs.tv_sec)
		return 1;
	return lhs.tv_nsec - rhs.tv_nsec;
}

void busy_wait(struct timespec t)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	struct timespec then = timespec_add(now, t);

	while(timespec_cmp(now, then) < 0)
	{
		for(int i = 0; i < 1000; i++){}
		clock_gettime(CLOCK_MONOTONIC, &now);
	}
}

void times(struct timespec t)
{

	clock_gettime(CLOCK_MONOTONIC, &t);
	t.nsec

	if (ns % 10000000 == 0)
	{
		printf("%d hundreths have passed\n", ns * 10000000);
	}
}

void busy_wait_with_times(struct timespec t)
{
	struct timespec now;
	//clock_gettime(CLOCK_MONOTONIC, &now);
	struct timespec then = timespec_add(now, t);

	while(timespec_cmp(now, then) < 0)
	{
		for(int i = 0; i < 1000; i++){}
		//clock_gettime(CLOCK_MONOTONIC, &now);
		times()
	}
}

int main()
{
	//usleep(2);
	//busy_wait(timespec_normalized(2, 0));
	busy_wait_with_times(timespec_normalized(2, 0));
	return 0;
}
