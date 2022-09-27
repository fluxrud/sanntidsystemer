#include <pthread.h>
// Note the argument and return types: void*
void* fn(void* args){
	return NULL;
}
int main(){
	pthread_t threadHandle;
	pthread_create(&threadHandle, NULL, fn, NULL);
	pthread_join(threadHandle, NULL);
}
