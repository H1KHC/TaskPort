#include "debug.h"
#include "worker.h"
#include "workerManager.h"

void workingProcess(Worker *worker) {
	std::unique_lock<std::mutex> lck(workers.waitWorkerMutex);
	while(!worker->terminateFlag) {
		tasks.getTaskFor(worker);
		if(worker->terminateFlag) break;
		void *result = worker->task->func(worker->task->param);
		tasks.submit(worker, worker->task, result);
		if(workers.checkIfNeedTerminate(worker)) break;
	}
	if(worker) workers.erase(worker);
	workers.workersMutex.lock();
	workers.workersMutex.unlock();
}