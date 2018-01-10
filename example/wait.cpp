#include "openTP.h"
#include <cstdio>
#include <cstdlib>
#include "debug.h"

void* count(void *x) {
	for(unsigned long i = 1; i <= (long)x; ++i)
		printf("%lu\n", i);
	return x;
}

void printResult(unsigned long id) {
	void*result;
	if(!tpGetTaskResult(id, &result))
		printf("Couldn't find task %lu's result\n", id);
	else printf("Task %lu returns with %lu\n", id, (unsigned long)result);
}

int main() {
	unsigned long taskID[2];
	taskID[0] = tpAddTask(count, (void*)3000L, 0);
	taskID[1] = tpAddTask(count, (void*)3000L, 0);
	tpSetCountOfWorkers();
	printf("Task added\n");
	printf("Start waiting\n");
	if(tpWaitForTaskFor(taskID[0], 500) == TASK_WAIT_TIME_OUT)
		printf("Task %d is too slow to wait.\n", taskID[0]);
	else printf("Task %d is fast enough to wait.\n", taskID[0]);
	printf("Start waiting for task %d\n", taskID[1]);
	tpWaitForAllTasks();
	printf("Tasks are finished, terminating...\n");
	tpSetCountOfWorkers(0);
	printf("Waiting for termination...\n");
	tpWaitForTermination();
	printf("Finished all task\n");
	printResult(taskID[0]);
	printResult(taskID[1]);
}