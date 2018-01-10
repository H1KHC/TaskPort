#include "openTP.h"
#include <cstdio>
#include <cstdlib>
#include "debug.h"

void* helloworld(void *x) {
	printf("Helloworld from %d\n", (int)x);
	return x;
}

int main() {
	unsigned long taskID[11];
	for(long i = 1; i <= 10; ++i)
		taskID[i] = tpAddTask(helloworld, (void*)i, -i);
	printf("Task added\n");
	tpSetCountOfWorkers();
	printf("Start waiting\n");
	tpWaitForAllTasks();
	printf("Tasks are finished, terminating...\n");
	tpSetCountOfWorkers(0);
	printf("Waiting for termination...\n");
	tpWaitForTermination();
	printf("Finished all task\n");
	for(long i = 1; i <= 10; ++i) {
		void *result;
		if(!tpGetTaskResult(taskID[i], &result))
			printf("Couldn't find task %lu's result\n", taskID[i]);
		printf("Task %lu returns with %lu\n", taskID[i], (unsigned long)result);
	}
}