#if defined(WIN32)
#include <windows.h>
#elif defined(__linux) || defined(SOLARIS) || defined(AIX)
#include <sys/sysinfo.h>
#else
#error This function is not implemented!
#endif

int getCPUCoreCount() {
#if defined(WIN32)
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
#elif defined(__linux) || defined(SOLARIS) || defined(AIX)
	return get_nprocs();
#endif
}