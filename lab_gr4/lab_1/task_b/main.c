#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>

#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/resource.h>

#include <sys/times.h>
uint64_t rdtsc(void){
	uint64_t val;
	asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(val) :: "memory");
	// You can check the current CPU frequency with $sudo dmesg | grep MHz
	return val;
}

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
		printf("Usage: ./main [0,1,2]\n");
		return 1;
	}

	int ns_max = 100;
	int histogram[ns_max];
	memset(histogram, 0, sizeof(int)*ns_max);
	int ns, t1, t2;
	struct timespec now1, now2;
	struct tms tms1, tms2;
	switch(atoi(argv[1]))
	{
	case 0:		
		for(int i = 0; i < 10*1000*1000; i++){
			t1 = rdtsc();
			t2 = rdtsc();
			ns = (t2 - t1);
			if(ns >= 0 && ns < ns_max){
				histogram[ns]++;
			}
		}
		break;
	case 1:
		for(int i = 0; i < 10*1000*1000; i++){
			clock_gettime(CLOCK_MONOTONIC, &now1);
			clock_gettime(CLOCK_MONOTONIC, &now2);
			t1 = now1.tv_nsec;
			t2 = now2.tv_nsec;
			ns = (t2 - t1);
			if(ns >= 0 && ns < ns_max){
				histogram[ns]++;
			}
		}
		break;
	case 2:
		for(int i = 0; i < 10*1000; i++){
			times(&tms1);
			times(&tms2);
			t1 = tms1.tms_utime;
			t2 = tms2.tms_utime;
			ns = (t2 - t1) * 10 * 1000 * 1000;
			if(ns >= 0 && ns < ns_max){
				histogram[ns]++;
			}
		}
		break;
	default:
		goto USAGE;
	}


	for(int i = 0; i < ns_max; i++){
		printf("%d\n", histogram[i]);
	}

	return 0;
}
