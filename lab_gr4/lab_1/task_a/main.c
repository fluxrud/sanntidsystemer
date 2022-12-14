#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>

#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/resource.h>

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
	//t.nsec

	int ns = 0;
	if ((ns / 1000000) % 10 == 0)
	{
		printf("%d hundreths have passed\n", ns * 10000000);
		printf("%d ns", ns);
	}
}

void busy_wait_with_getrusage(struct timespec t)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	struct timespec then = timespec_add(now, t);
	
	struct rusage usage;
	
	while(timespec_cmp(now, then) < 0)
	{
		for(int i = 0; i < 1000; i++){}
		clock_gettime(CLOCK_MONOTONIC, &now);
		getrusage(RUSAGE_SELF, &usage);
	}
}
int main(int argc, const char* argv[])
{
	if(argc != 2)
	{
		USAGE:
		printf("Usage: ./main [0,1,2], (0 for usleep(), 1 for busy_wait(), and 2 for busy_wait with getrusage) \n");
		return 0;
	}
	switch(atoi(argv[1]))
	{
	case 0:
		usleep(2000000);
		break;
	case 1:
		busy_wait(timespec_normalized(2, 0));
		break;
	case 2:
		busy_wait_with_getrusage(timespec_normalized(2, 0));
		break;
	default:
		goto USAGE;
	}
	return 0;
}
