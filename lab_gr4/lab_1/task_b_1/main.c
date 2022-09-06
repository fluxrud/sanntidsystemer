#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>

#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/resource.h>

#include <sys/times.h>

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

uint64_t rdtsc(void){
	uint64_t val;
	asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(val) :: "memory");
	// You can check the current CPU frequency with $sudo dmesg | grep MHz
	return val;
}

int main(int argc, const char* argv[])
{
	if(argc != 2)
	{
		USAGE:
		printf("Usage: ./main [0,1,2]\n");
		return 1;
	}
	int t1;
	struct timespec now1, now2;
	struct tms tms1, tms2;
	int n_runs = 10 * 1000 * 1000;
	switch(atoi(argv[1]))
	{
	case 0:		
		clock_gettime(CLOCK_MONOTONIC, &now1);
		for(int i = 0; i < n_runs; i++)
			t1 = rdtsc();
		clock_gettime(CLOCK_MONOTONIC, &now2);
		printf("rdtsc() average access time: %d \n", 18 * timespec_sub(now2, now2).tv_nsec / n_runs);
	case 1:
		clock_gettime(CLOCK_MONOTONIC, &now1);
		for(int i = 0; i < n_runs; i++)
			clock_gettime(CLOCK_MONOTONIC, &now1);
		clock_gettime(CLOCK_MONOTONIC, &now2);
		printf("clock_gettime() average access time: %d \n", timespec_sub(now2, now2).tv_nsec / n_runs);
	case 2:
		clock_gettime(CLOCK_MONOTONIC, &now1);
		for(int i = 0; i < n_runs / 1000; i++)
			times(&tms1);
		clock_gettime(CLOCK_MONOTONIC, &now2);
		printf("times() average access time: %d \n", timespec_sub(now2, now2).tv_nsec / n_runs / 1000);
	default:
		break;
	}

	return 0;
}
