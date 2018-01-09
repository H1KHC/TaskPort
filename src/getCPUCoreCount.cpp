#if defined(WIN32)

#include <windows.h>

int getCPUCoreCount() {
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
}

#elif defined(__linux) || defined(__CYGWIN__)
#include <sys/sysinfo.h>

int getCPUCoreCount() {
	return get_nprocs();
}

#else
#error Unluckily, the function getCPUCoreCount is not implemented on your \
	platform. If your platform support one method of the above or if you have \
	thought about a method, please contact me or pull a request.
#endif