

// number of philosophers
#define n 4

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// phil. sync.
pthread_barrier_t barrier;

pthread_mutex_t forks[n];

uint8_t l_fork(uint8_t i){ return i; }
uint8_t r_fork(uint8_t i){ return (i + 1) % n; }

void p(uint8_t p_id)
{
	uint8_t i = 10;
	while(i--){
		printf("p: %i is %i years old\n", p_id, 10 - i);		

		// phil. sync.
		pthread_barrier_wait(&barrier);
		
		printf("p: %i thinking before left \n", p_id);
		pthread_mutex_lock(&forks[l_fork(p_id)]);
		sleep(1);		
		printf("p: %i thinking before right \n", p_id);
		pthread_mutex_lock(&forks[r_fork(p_id)]);
		
		printf("p: %i eating\n", p_id);
		sleep(1);
		
		pthread_mutex_unlock(&forks[l_fork(p_id)]);
		pthread_mutex_unlock(&forks[r_fork(p_id)]);
	}
	printf("p: %i is dead\n", p_id);
}

void p_rh(uint8_t p_id)
{
	uint8_t i = 10;
	while(i--){
		printf("p: %i is %i years old\n", p_id, 10 - i);		

		// phil. sync.
		pthread_barrier_wait(&barrier);
		
		printf("p: %i thinking before left \n", p_id);
		if (p_id == n-1){
			pthread_mutex_lock(&forks[r_fork(p_id)]);
		} else {
			pthread_mutex_lock(&forks[l_fork(p_id)]);
		}
		sleep(1);		
		printf("p: %i thinking before right \n", p_id);
		if (p_id == n-1){
			pthread_mutex_lock(&forks[l_fork(p_id)]);
		} else {
			pthread_mutex_lock(&forks[r_fork(p_id)]);
		}
		
		printf("p: %i eating\n", p_id);
		sleep(1);
		
		pthread_mutex_unlock(&forks[l_fork(p_id)]);
		pthread_mutex_unlock(&forks[r_fork(p_id)]);
	}
	printf("p: %i is dead\n", p_id);
}
int main()
{
	// phil. sync.
	pthread_barrier_init(&barrier, NULL, n);

	pthread_t threadHandles[n];	
	for (uint8_t i = 0;  i < n; i++){
		pthread_create(&threadHandles[i], NULL, (void*(*)(void*))p_rh, (void*)i);
	}
	
	for (uint8_t i = 0;  i < n; i++){
		pthread_join(threadHandles[i], NULL);
	}
}
