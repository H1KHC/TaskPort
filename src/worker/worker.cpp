#include "debug.h"
#include "worker.h"
#include "workerManager.h"

void workingProcess(Worker *worker) {
	while(!worker->terminateFlag) {
		tasks.getTaskFor(worker);
		if(worker->terminateFlag) break;
		void *result = worker->task->func(worker->task->param);
		if(workers.checkIfNeedTerminate(worker)) break;
		tasks.submit(worker, worker->task, result);
	}
	if(worker) workers.erase(worker);
}