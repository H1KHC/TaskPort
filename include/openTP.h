#ifndef __OPENTP_H__
#define __OPENTP_H__

#ifdef TP_STATIC
#   define TPAPI
#elif defined(WIN32)
#   ifdef TP_IMPLEMENT
#       define TPAPI __declspec(dllexport)
#   else
#       define TPAPI __declspec(dllimport)
#   endif
#else
#   define TPAPI __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
#define DEFAULT(x)  = (x)
#else
#define DEFAULT(...)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*taskFunction)(void *);

#define DEFAULT_COUNT_OF_WORKERS			(-1)
TPAPI void tpSetCountOfWorkers(int count DEFAULT(DEFAULT_COUNT_OF_WORKERS));
TPAPI void tpTerminate();
TPAPI void tpWaitForTermination();

#define INVALID_TASK_ID						(-1)
TPAPI long tpAddTask(taskFunction func, void *param, int priority DEFAULT(0));
//TPAPI int cancelTask(int tid, int force);
//TPAPI int waitForTask(int tid);
//TPAPI int waitForTaskFor(int tid, long milli);
TPAPI void tpWaitForTasks();

#define TASK_NOT_FOUND				(-1)
#define TASK_STATE_QUEUING			0
#define TASK_STATE_ASSIGNED			1
#define TASK_STATE_FINISHED			2
#define TASK_STATE_DETACHED			3
TPAPI int tpGetTaskState(long tid);

TPAPI int tpGetTaskResult(long tid, void**result);

#ifdef __cplusplus
}
#endif

#endif