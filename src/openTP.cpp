#include "task/taskManager.h"
#include "worker/workerManager.h"

void tpSetCountOfWorkers(int count) {
	if(DEFAULT_COUNT_OF_WORKERS == count) {
		int getCPUCoreCount();
		count = std::thread::hardware_concurrency();
		if(!count) count = getCPUCoreCount();
	}
	if(count > 0) workers.terminated = false;
	else workers.terminateAll();
	if(workers.maxWorker < count) {
		int delta = count - workers.maxWorker;
		workers.maxWorker = count;
		while(delta--)
			workers.addWorker();
	} else workers.maxWorker = count;
}

void tpTerminate() {
	tpSetCountOfWorkers(0);
}

void tpWaitForTermination() {
	workers.waitForTermination();
}

long tpAddTask(taskFunction func, void *param, int priority) {
	return tasks.add(func, param, priority);
}

int tpWaitForTask(int tid) {
	return tasks.wait(tid);
}

int tpWaitForTaskFor(int tid, long microseconds) {
	return tasks.waitFor(tid, microseconds);
}

void tpWaitForAllTasks() {
	tasks.waitForTasks();
}

int tpGetTaskState(long tid) {
	return tasks.getState(tid);
}

int tpGetTaskResult(long tid, void **result) {
	return tasks.getResult(tid, result);
}