// clang -lpthread -std=gnu11 main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <float.h>

#define timeUnitUs 100 * 1000 // about 100 ms

pthread_mutex_t taskStateMutex;

enum taskState {idle, running, preempted, finished};
const char *taskStateString[4] = {"idle", "running", "preempted", "finished"};

struct Task {
	enum taskState state;
	int ID;
	int startTime; // In some imaginary integer time unit
	int totalRuntime; // In some imaginary integer time unit
	int currentRuntime; // In some imaginary integer time unit
};


void set_task_state(struct Task * task, enum taskState taskNewState){
	pthread_mutex_lock(&taskStateMutex);
	task->state = taskNewState;
	pthread_mutex_unlock(&taskStateMutex);
}

void *task_handler(void *var) {
	struct Task * task;
	task = (struct Task *) var;
	enum taskState prevTaskState = task->state;
	printf("Task handler initiated for task with ID %d in state %s \n", task->ID, taskStateString[task->state]);
	fflush(stdout);
	while(task->currentRuntime < task->totalRuntime){
		if(task->state != prevTaskState){
			enum taskState taskOldState = prevTaskState;
			prevTaskState = task->state;
			printf("Task with ID %d changed state from %s to %s after having worked for %d time units \n", task->ID, taskStateString[taskOldState], taskStateString[task->state], task->currentRuntime);
			fflush(stdout);
		}
		if(task->state == running){
			task->currentRuntime ++;
		}
		usleep(timeUnitUs);
	}
	printf("Task with ID %d finising after having worked for %d out of %d time units \n", task->ID, task->currentRuntime, task->totalRuntime);
	fflush(stdout);
	set_task_state(task, finished);
	return NULL;
}


void wait_for_rescheduling(int tick_time_units, struct Task* task){
	int runtime = 0;
	while(task->state != finished && runtime < tick_time_units){
		usleep(timeUnitUs);
		runtime++;
	}
}

void round_robin(struct Task ** tasks, int taskCount, int timeout, int interval){
	printf("Started RR scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int prevTaskIndex = -1;
	int counter = 0;
	while(1){
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);
		
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}
		
		counter = 0;
		while(counter < taskCount){
			currentTaskIndex = (currentTaskIndex + 1) % taskCount;
			if(tasks[currentTaskIndex]->state != finished && tasks[currentTaskIndex]->startTime <= totalTimePassed){
				break;
			}
			counter ++;
		}

		if(counter >= taskCount){
			totalTimePassed ++; //Speeding through areas without tasks to schedule
		} else {
			if(prevTaskIndex != currentTaskIndex && prevTaskIndex != -1 && tasks[prevTaskIndex]->state != finished){ // prevTaskIndex == -1 means that this is the first task to be selected
				set_task_state(tasks[prevTaskIndex], preempted);
			}
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(interval, tasks[currentTaskIndex]);
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
			prevTaskIndex = currentTaskIndex;
		}
	}
}

// Implement your schedulers here!

void fcfs(struct Task ** tasks, int taskCount, int timeout, int interval){
	printf("Started fcfs scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int prevTaskIndex = -1;
	int counter = 0;
	while(1){
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);
		
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}
		
		counter = 0;
		while(counter < taskCount){
			if(tasks[currentTaskIndex]->state == finished && currentTaskIndex < taskCount - 1){
				currentTaskIndex++;
			}
			if(tasks[currentTaskIndex]->state != finished && tasks[currentTaskIndex]->startTime <= totalTimePassed){
				break;
			}
			counter ++;
		}

		if(counter >= taskCount){
			totalTimePassed ++; //Speeding through areas without tasks to schedule
		} else {
			if(prevTaskIndex != currentTaskIndex && prevTaskIndex != -1 && tasks[prevTaskIndex]->state != finished){ // prevTaskIndex == -1 means that this is the first task to be selected
				set_task_state(tasks[prevTaskIndex], preempted);
			}
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(interval, tasks[currentTaskIndex]);
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
			prevTaskIndex = currentTaskIndex;
		}
	}
}


void spn(struct Task ** tasks, int taskCount, int timeout, int interval){
	printf("Started spn scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int prevTaskIndex = -1;
	int counter = 0;
	while(1){
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);
		
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}
		
		counter = 0;
		while(counter < taskCount){
			if(tasks[currentTaskIndex]->state == finished){
				uint8_t min = 255;
				uint8_t index = 0;
				for (int i = 0; i < taskCount; i++){
					if(tasks[i]->state == idle && tasks[i]->startTime <= totalTimePassed){
						if(tasks[i]->totalRuntime < min){
							min = tasks[i]->totalRuntime;
							index = i;
						}
					}
				}
				if(min < 255) currentTaskIndex = index;
			}
			if(tasks[currentTaskIndex]->state != finished && tasks[currentTaskIndex]->startTime <= totalTimePassed){
				break;
			}
			counter ++;
		}

		if(counter >= taskCount){
			totalTimePassed ++; //Speeding through areas without tasks to schedule
		} else {
			if(prevTaskIndex != currentTaskIndex && prevTaskIndex != -1 && tasks[prevTaskIndex]->state != finished){ // prevTaskIndex == -1 means that this is the first task to be selected
				set_task_state(tasks[prevTaskIndex], preempted);
			}
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(interval, tasks[currentTaskIndex]);
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
			prevTaskIndex = currentTaskIndex;
		}
	}
}


void hrrn(struct Task ** tasks, int taskCount, int timeout, int interval){
	printf("Started spn scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int prevTaskIndex = -1;
	int counter = 0;
	while(1){
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);
		
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}
		
		counter = 0;
		while(counter < taskCount){
			if(tasks[currentTaskIndex]->state == finished){
				float min_rr = FLT_MAX;
				uint8_t index = 0;
				for (int i = 0; i < taskCount; i++){
					if(tasks[i]->state == idle && tasks[i]->startTime <= totalTimePassed){
						float rr = ((totalTimePassed - tasks[i]->startTime + tasks[i]->totalRuntime) / (float)(totalTimePassed - tasks[i]->startTime));
						if(rr < min_rr){
							min_rr = rr;
							index = i;
						}
					}
				}
				if(min_rr < FLT_MAX) currentTaskIndex = index;
			}
			if(tasks[currentTaskIndex]->state != finished && tasks[currentTaskIndex]->startTime <= totalTimePassed){
				break;
			}
			counter ++;
		}

		if(counter >= taskCount){
			totalTimePassed ++; //Speeding through areas without tasks to schedule
		} else {
			if(prevTaskIndex != currentTaskIndex && prevTaskIndex != -1 && tasks[prevTaskIndex]->state != finished){ // prevTaskIndex == -1 means that this is the first task to be selected
				set_task_state(tasks[prevTaskIndex], preempted);
			}
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(interval, tasks[currentTaskIndex]);
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
			prevTaskIndex = currentTaskIndex;
		}
	}
}
//preemptive schedulers 

void srt(struct Task ** tasks, int taskCount, int timeout, int interval){
	printf("Started srt scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int prevTaskIndex = -1;
	int counter = 0;
	while(1){
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);
		
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}
		
		counter = 0;
		while(counter < taskCount){
			uint8_t min = 255;
			uint8_t index = 0;
			for (int i = 0; i < taskCount; i++){
				if((tasks[i]->state == idle || tasks[i]->state == preempted || tasks[i]->state == running) && tasks[i]->startTime <= totalTimePassed){
					uint8_t rt = tasks[i]->totalRuntime - tasks[i]->currentRuntime;
					if(rt < min){
						min = rt;
						index = i;
					}
				}
			}
			if(min < 255) currentTaskIndex = index;
			if(tasks[currentTaskIndex]->state != finished && tasks[currentTaskIndex]->startTime <= totalTimePassed){
				break;
			}
			counter ++;
		}

		if(counter >= taskCount){
			totalTimePassed ++; //Speeding through areas without tasks to schedule
		} else {
			if(prevTaskIndex != currentTaskIndex && prevTaskIndex != -1 && tasks[prevTaskIndex]->state != finished){ // prevTaskIndex == -1 means that this is the first task to be selected
				set_task_state(tasks[prevTaskIndex], preempted);
			}
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(interval, tasks[currentTaskIndex]);
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
			prevTaskIndex = currentTaskIndex;
		}
	}
}


struct queue_t {
	int id;
	int pri;
};

static int compare(const void * p1, const void * p2){
	int pri1 = ((struct queue_t*)p1)->pri;	
	int pri2 = ((struct queue_t*)p2)->pri;	

	// return 0 if p1 pri = p2 pri
	if(pri1 == pri2)return 0;
	// return -1 if p1 pri < p2 pri
	else if (pri1 < pri2) return -1;
	// return 1 else
	else return 1;
}

void f(struct Task ** tasks, int taskCount, int timeout, int interval){
	printf("Started f scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int prevTaskIndex = -1;
	int counter = 0;

	// queue
	
	struct queue_t queue[taskCount];
	for (int i = 0; i < taskCount; i++){		
		queue[i].pri = 0;
		queue[i].id = tasks[i]->ID;
	}

	while(1){
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);
		
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}
		
		counter = 0;
		while(counter < taskCount){
			// preempt
			// if current task not finished, increase pri
			//if(tasks[currentTaskIndex]->state != finished) queue[0].pri += 10;
			
			qsort(queue, taskCount, sizeof(queue[0]), compare);
			for (int i = 0; i < taskCount; i++){
				if((tasks[queue[i].id]->state != finished) && tasks[queue[i].id]->startTime <= totalTimePassed){
					currentTaskIndex = queue[i].id;
					queue[i].pri += 10;
					printf("increased priority of id:  %d pri: %d tot time: %d\n", queue[i].id, queue[i].pri, totalTimePassed);
					break;
				}
			}
			if(tasks[currentTaskIndex]->state != finished && tasks[currentTaskIndex]->startTime <= totalTimePassed){
				break;
			}
			counter ++;
		}

		if(counter >= taskCount){
			totalTimePassed ++; //Speeding through areas without tasks to schedule
		} else {
			if(prevTaskIndex != currentTaskIndex && prevTaskIndex != -1 && tasks[prevTaskIndex]->state != finished){ // prevTaskIndex == -1 means that this is the first task to be selected
				set_task_state(tasks[prevTaskIndex], preempted);
			}
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(interval, tasks[currentTaskIndex]);
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
			prevTaskIndex = currentTaskIndex;
		}
	}
}



int main(){
	printf("Main started \n");
	fflush(stdout);
	int taskCount = 5;
	pthread_t threads[taskCount];
	
	// {state, ID, start-time, total runtime, current runtime}
	struct Task task0 = {idle, 0, 00, 30, 0};
	struct Task task1 = {idle, 1, 15, 60, 0};
	struct Task task2 = {idle, 2, 35, 40, 0};
	struct Task task3 = {idle, 3, 55, 50, 0};
	struct Task task4 = {idle, 4, 75, 20, 0};
	
	struct Task * tasks[] = {&task0, &task1, &task2, &task3, &task4};
	
	for(int i = 0; i < taskCount; i++){
		pthread_create(&threads[i], NULL, task_handler, (void*) tasks[i]);
	}
	
	sleep(1); // Let everything stabilize
	int schedulerTimeout = 250;
	
    f(tasks, taskCount, schedulerTimeout, 10);

	
	for(int i = 0; i < taskCount; i++){
		pthread_join(threads[i], NULL);
	}
	
	printf("Main done \n");
	fflush(stdout);
	return 0;
}
