#include <chrono>
#include "openTP.h"
#include "debug.h"
#include "worker/worker.h"
#include "taskManager.h"

TaskManager::TaskManager() : IDTail(1) {}

long TaskManager::add(taskFunction f, void *p, int priority) {
	taskManagerMutex.lock();
	unsigned long id = IDTail++;
	Task *t = new Task(id, priority, f, p);

	existingTasks.insert(t);
	pendingTasks.insert(t);
	waitForNewTask.notify_one();
	taskManagerMutex.unlock();
	return id;
}

int TaskManager::wait(int ID) {
	std::unique_lock<std::mutex> lck(waitForExistingTaskMutex);
	int state = getState(ID);
	if(ID == TASK_STATE_FINISHED) return TASK_WAIT_SUCCESSFUL;
	if(ID == TASK_NOT_FOUND) return INVALID_TASK_ID;
	while(!(getState(ID) == TASK_STATE_FINISHED))
		waitForExistingTask.wait_for(lck, std::chrono::microseconds(1));
	return TASK_WAIT_SUCCESSFUL;
}

int TaskManager::waitFor(int ID, long micro) {
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::microseconds microseconds;
	std::unique_lock<std::mutex> lck(waitForExistingTaskMutex);

	clock::time_point startTime(clock::now());
	microseconds timeToWait(micro);

	int state = getState(ID);
	if(ID == TASK_STATE_FINISHED) return TASK_WAIT_SUCCESSFUL;
	if(ID == TASK_NOT_FOUND) return INVALID_TASK_ID;
	
	while(clock::now() - startTime < timeToWait) {
		if(getState(ID) == TASK_STATE_FINISHED) return TASK_WAIT_SUCCESSFUL;
		waitForExistingTask.wait_for(lck, std::chrono::microseconds(1));
	}
	return TASK_WAIT_TIME_OUT;
}

void TaskManager::getTaskFor(Worker *worker) {
	std::unique_lock<std::mutex> lck(waitForNewTaskMutex);
	while(true) {
		taskManagerMutex.lock();
		if(!pendingTasks.empty() || worker->terminateFlag) break;
		taskManagerMutex.unlock();
		waitForNewTask.wait_for(lck, std::chrono::microseconds(1));
	}
	if(worker->terminateFlag) {
		if(!pendingTasks.empty())
			waitForNewTask.notify_one();
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
	if(task->state == TASK_STATE_ASSIGNED)
		finishedTasks.insert(std::make_pair(task->ID, result));
	taskManagerMutex.unlock();
	delete task;
	waitForExistingTask.notify_all();
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
	std::unique_lock<std::mutex> lck(waitForExistingTaskMutex);
	while(true) {
		taskManagerMutex.lock();
		if(existingTasks.size() == 0) {
			taskManagerMutex.unlock();
			return;
		}
		taskManagerMutex.unlock();
		waitForExistingTask.wait_for(lck, std::chrono::microseconds(1));
	}
}

TaskManager tasks;
