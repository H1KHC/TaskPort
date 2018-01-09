#include "debug.h"
#include "worker/worker.h"
#include "taskManager.h"

TaskManager::TaskManager() :IDTail(1), countPendingTasks(0) {}

long TaskManager::add(taskFunction f, void *p, int priority) {
	Task *t = new Task(IDTail++, priority, f, p);

	pendingTasksMutex.lock();
	pendingTasks.insert(t);
	pendingTasksMutex.unlock();
	
	existingTasksMutex.lock();
	existingTasks.insert(t);
	++countPendingTasks;
	existingTasksMutex.unlock();

	waitTask.notify_one();

	return t->ID;
}

void TaskManager::getTaskFor(Worker *worker) {
	std::unique_lock<std::mutex> lck(waitTaskMutex);
	while(true) {
		pendingTasksMutex.lock();
		if(!pendingTasks.empty() || worker->terminateFlag) break;
		pendingTasksMutex.unlock();
		waitTask.wait(lck);
	}
	if(worker->terminateFlag) {
		if(!pendingTasks.empty())
			waitTask.notify_one();
		pendingTasksMutex.unlock();
		return;
	}
	auto it = pendingTasks.begin();
	Task *task = *it;
	(*it)->state = TASK_STATE_ASSIGNED;
	pendingTasks.erase(it);
	pendingTasksMutex.unlock();
	task->owner = worker;
	worker->task = task;
}

void TaskManager::submit(Worker *worker, Task *task, void *result) {
	worker->task = nullptr;
	existingTasksMutex.lock();
	existingTasks.erase(task);
	--countPendingTasks;
	existingTasksMutex.unlock();
	if(task->state == TASK_STATE_ASSIGNED) {
		finishedTasksMutex.lock();
		// log("Task %d completes with value %lu\n", task->ID, (unsigned long)result);
		finishedTasks.insert(std::make_pair(task->ID, result));
		finishedTasksMutex.unlock();
	}
	delete task;
	waitForAllTasks.notify_all();
}

bool TaskManager::getResult(int ID, void **result) {
	finishedTasksMutex.lock();
	auto it = finishedTasks.find(ID);
	if(it == finishedTasks.end()) {
		finishedTasksMutex.unlock();
		return false;
	}
	*result = it->second;
	finishedTasks.erase(it);
	finishedTasksMutex.unlock();
	return true;
}

int TaskManager::getState(int ID) {
	finishedTasksMutex.lock();
	if(finishedTasks.find(ID) != finishedTasks.end()) {
		finishedTasksMutex.unlock();
		return TASK_STATE_FINISHED;
	}
	finishedTasksMutex.unlock();

	auto toFind = new Task(ID);
	existingTasksMutex.lock();
	auto it = existingTasks.find(toFind);
	delete toFind;
	if(it == existingTasks.end()) {
		existingTasksMutex.unlock();
		return TASK_NOT_FOUND;
	}
	int state = (*it)->state;
	existingTasksMutex.unlock();
	return state;
}

void TaskManager::waitForTasks() {
	std::unique_lock<std::mutex> lck(waitForAllTasksMutex);
	while(true) {
		existingTasksMutex.lock();
		if(countPendingTasks == 0) {
			existingTasksMutex.unlock();
			return;
		}
		existingTasksMutex.unlock();
		waitForAllTasks.wait(lck);
	}
}

TaskManager tasks;
