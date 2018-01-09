#ifndef __TASK_MANAGER_H__
#define __TASK_MANAGER_H__

#include <set>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <condition_variable>
#include "debug.h"
#include "task.h"

class TaskManager {
public:
	struct taskSortPriorityFirst{bool operator() (const Task *a, const Task *b) const {
		return	a->priority < b->priority ? true : (
				a->priority > b->priority ? false : (
				a->createTime < b->createTime
				));
	}};

	struct taskSortIDFirst{bool operator() (const Task *a, const Task *b) const {
		return a->ID < b->ID;
	}};

private:
	friend class WorkerManager;
	std::atomic_ulong IDTail, countPendingTasks;
	std::mutex pendingTasksMutex, existingTasksMutex, finishedTasksMutex;
	std::mutex waitTaskMutex, waitForAllTasksMutex;
	std::condition_variable waitTask, waitForAllTasks;
	std::set<Task *, taskSortPriorityFirst> pendingTasks;
	std::set<Task *, taskSortIDFirst> existingTasks;
	std::unordered_map<int, void*> finishedTasks;

protected:
	friend void workingProcess(Worker *worker);
	// pull a task request
	long add(taskFunction f, void *p, int priority);
	friend long tpAddTask(taskFunction func, void *param, int priority);

	// If the task to cancel is queuing, it will be erased
	// Else if it is running, it will be either detached or terminated
	//   depends on whether flag force is set or not
	// bool cancelTask(int ID, bool force);	//UNIMPLEMENTED
	// friend int cancelTask(int, int);

	// Wait until the task specified is finished
	// bool waitForTask(int ID);
	// friend int waitForTask(int);

	// Get a pending task
	// Notice that this will lock the thread until a task is available
	void getTaskFor(Worker *worker);

	// Submit a compileted task
	void submit(Worker *worker, Task *task, void *result);

public:
	// Find a finished task and get its result
	// This will delete the task completely
	bool getResult(int ID, void **result);

	// Get a task's state
	int getState(int ID);

	void waitForTasks();

	TaskManager();
};

extern TaskManager tasks;

#endif