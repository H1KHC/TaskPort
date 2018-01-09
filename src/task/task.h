#ifndef __TASK_H__
#define __TASK_H__

#include <chrono>
#include "openTP.h"

struct Worker;
struct Task {
	unsigned long long ID;
	int state, priority;
	std::chrono::high_resolution_clock::time_point createTime;
	taskFunction func;
	void *param;
	Worker *owner;
	Task(unsigned long long id, int prio, taskFunction f, void *p) :
		ID(id), state(TASK_STATE_QUEUING), priority(prio),
		createTime(std::chrono::high_resolution_clock::now()),
		func(f), param(p) {}

	Task(unsigned long long id) : ID(id) {} // Only used to find
};

#endif