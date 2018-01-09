#ifndef __WORKER_MANAGER_H__
#define __WORKER_MANAGER_H__

#include <mutex>
#include <condition_variable>
#include "worker.h"
#include "task/taskManager.h"

class WorkerManager {
	bool terminated;
	friend void tpSetCountOfWorkers(int count);
	std::mutex workersMutex, waitWorkerMutex;
	std::condition_variable waitWorker;
	int maxWorker, detachedWorker;
	unsigned long long IDTail;
	std::list<Worker *> workers;
protected:
	friend void workingProcess(Worker *worker);
	void addWorker();
	bool checkIfNeedTerminate(Worker *&worker);

	// Only pending workers can call this function
	void erase(Worker *worker);
public:
	bool isTerminated() { return terminated; }
	void setMaxWorker();
	void terminateAll();

	void waitForTermination() {
		std::unique_lock<std::mutex> lck(waitWorkerMutex);
		while(true) {
			workersMutex.lock();
			if(workers.empty()) {
				workersMutex.unlock();
				return;
			}
			workersMutex.unlock();
			waitWorker.wait(lck);
		}
	}

	WorkerManager();
};

extern WorkerManager workers;
#endif