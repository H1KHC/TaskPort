#include "openTP.h"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

void* helloworld(void *x) {
	printf("Helloworld from %d\n", (int)x);
	return x;
}

int taskID[101];

int main() {
	for(int i = 1; i <= 100; ++i)
		taskID[i] = tpAddTask(helloworld, (void*)i, -i);
	tpSetCountOfWorkers();
	tpWaitForTasks();
	tpSetCountOfWorkers(0);
	tpWaitForTermination();
	for(int i = 1; i <= 100; ++i) {
		long ret;
		tpGetTaskResult(i, (void**)(&ret));
		printf("Task %d returns %d\n", taskID[i], ret);
	}

	tpSetCountOfWorkers();
	for(int i = 100; i >= 1; --i)
		taskID[i] = tpAddTask(helloworld, (void*)i, i);
	tpWaitForTasks();
	tpTerminate();
	tpWaitForTermination();
	for(int i = 100; i >= 1; --i) {
		long ret;
		tpGetTaskResult(i, (void**)(&ret));
		printf("Task %d returns %d\n", taskID[i], ret);
	}
}