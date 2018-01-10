#ifndef __TASK_MANAGER_H__
#define __TASK_MANAGER_H__

#include <set>
#include <mutex>
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
	int IDTail;
	std::mutex taskManagerMutex;
	std::mutex waitForNewTaskMutex, waitForExistingTaskMutex;
	std::condition_variable waitForNewTask, waitForExistingTask;
	std::multiset<Task *, taskSortPriorityFirst> pendingTasks;
	std::multiset<Task *, taskSortIDFirst> existingTasks;
	std::unordered_map<int, void*> finishedTasks;
	friend void workingProcess(Worker *worker);

public:
	// pull a task request
	long add(taskFunction f, void *p, int priority);

	int wait(int ID);
	int waitFor(int ID, long microseconds);

protected:
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
	bool getResult(long ID, void **result);

	// Get a task's state
	int getState(long ID);

	void waitForTasks();

	TaskManager();
};

extern TaskManager tasks;

#endif