#include <chrono>
#include "debug.h"
#include "worker/worker.h"
#include "taskManager.h"

TaskManager::TaskManager() : IDTail(1), countExistingTasks(0) {}

long TaskManager::add(taskFunction f, void *p, int priority) {
	taskManagerMutex.lock();
	unsigned long id = IDTail++;
	Task *t = new Task(id, priority, f, p);

	pendingTasks.insert(t);
	existingTasks.insert(t);
	++countExistingTasks;

	waitTask.notify_one();
	taskManagerMutex.unlock();
	return id;
}

void TaskManager::getTaskFor(Worker *worker) {
	std::unique_lock<std::mutex> lck(waitTaskMutex);
	while(true) {
		taskManagerMutex.lock();
		if(!pendingTasks.empty() || worker->terminateFlag) break;
		taskManagerMutex.unlock();
		waitTask.wait_for(lck, std::chrono::microseconds(1));
	}
	if(worker->terminateFlag) {
		if(!pendingTasks.empty())
			waitTask.notify_one();
		taskManagerMutex.unlock();
		return;
	}
	auto it = pendingTasks.begin();
	Task *task = *it;
	(*it)->state = TASK_STATE_ASSIGNED;
	pendingTasks.erase(it);
	taskManagerMutex.unlock();
	task->owner = worker;
	worker->task = task;
}

void TaskManager::submit(Worker *worker, Task *task, void *result) {
	worker->task = nullptr;
	taskManagerMutex.lock();
	existingTasks.erase(task);
	--countExistingTasks;
	if(task->state == TASK_STATE_ASSIGNED)
		finishedTasks.insert(std::make_pair(task->ID, result));
	taskManagerMutex.unlock();
	delete task;
	waitForAllTasks.notify_all();
}

bool TaskManager::getResult(long ID, void **result) {
	taskManagerMutex.lock();
	auto it = finishedTasks.find(ID);
	if(it == finishedTasks.end()) {
		taskManagerMutex.unlock();
		return false;
	}
	*result = it->second;
	finishedTasks.erase(it);
	taskManagerMutex.unlock();
	return true;
}

int TaskManager::getState(long ID) {
	taskManagerMutex.lock();
	if(finishedTasks.find(ID) != finishedTasks.end()) {
		taskManagerMutex.unlock();
		return TASK_STATE_FINISHED;
	}

	auto toFind = new Task(ID);
	auto it = existingTasks.find(toFind);
	delete toFind;
	if(it == existingTasks.end()) {
		taskManagerMutex.unlock();
		return TASK_NOT_FOUND;
	}
	int state = (*it)->state;
	taskManagerMutex.unlock();
	return state;
}

void TaskManager::waitForTasks() {
	std::unique_lock<std::mutex> lck(waitForAllTasksMutex);
	while(true) {
		taskManagerMutex.lock();
		if(countExistingTasks == 0) {
			taskManagerMutex.unlock();
			return;
		}
		taskManagerMutex.unlock();
		waitForAllTasks.wait_for(lck, std::chrono::microseconds(1));
	}
}

TaskManager tasks;
