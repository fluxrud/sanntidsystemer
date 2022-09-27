#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
pthread_barrier_t barr;
// Note the argument and return types: void*
long a = 0;
sem_t a_sem;
void* fn(void* args){
	long b = 0;
	
	for (int i; i<1000000; i++){
	  	sem_wait(&a_sem);
		a++;
	  	b++;
		sem_post(&a_sem);
	}
	int relise= pthread_barrier_wait(&barr); 
	sem_wait(&a_sem);
	printf("a = %li , b = %li \n", a,b);
	sem_post(&a_sem);
	return NULL;
}
void  taskA(){
	pthread_t threadHandle1, threadHandle2;
	pthread_barrier_init (&barr, NULL, 2);
	pthread_create(&threadHandle1, NULL, fn, NULL);
	//sleep(2);
	pthread_create(&threadHandle2, NULL, fn, NULL);
	pthread_join(threadHandle1,  NULL);
	pthread_join(threadHandle2,  NULL);
}
int main(){
	sem_init (&a_sem,0,1);
	taskA(); 
}

