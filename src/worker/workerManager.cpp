#include "debug.h"
#include "worker.h"
#include "workerManager.h"
#include "task/taskManager.h"

WorkerManager::WorkerManager() :
	terminated(true), maxWorker(0), detachedWorker(0), IDTail(1) {}

void WorkerManager::addWorker() {
	Worker *worker = new Worker(IDTail++);
	workersMutex.lock();
	workers.push_back(worker);
	workersMutex.unlock();
	worker->thisIterator = (++workers.rbegin()).base();
}

bool WorkerManager::checkIfNeedTerminate(Worker *&worker) {
	workersMutex.lock();
	int size = workers.size();
	workersMutex.unlock();
	if(size <= maxWorker) return false;
	erase(worker), worker = nullptr;
	return true;
}

void WorkerManager::erase(Worker *worker) {
	workersMutex.lock();
	workers.erase(worker->thisIterator);
	workersMutex.unlock();
	detachedWorker--;
	worker->workingThread.detach();
	delete worker;
	waitWorker.notify_all();
}

void WorkerManager::terminateAll() {
	terminated = true;
	workersMutex.lock();
	for(Worker *worker : workers) worker->terminateFlag = true;
	workersMutex.unlock();
	tasks.waitTask.notify_all();
}

WorkerManager workers;