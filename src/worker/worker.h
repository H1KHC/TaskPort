#ifndef __WORKER_H__
#define __WORKER_H__

#include <thread>
#include <list>

struct Task;
struct Worker;
void workingProcess(Worker *);

struct Worker {
	std::list<Worker *>::iterator thisIterator;
	bool terminateFlag;
	Task *task;
	unsigned long long ID;
	std::thread workingThread;

	Worker(int id) : terminateFlag(0), task(nullptr), ID(id),
		workingThread(workingProcess, this) {}
};
#endif