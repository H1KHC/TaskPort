#ifndef __DEBUG_H__
#define __DEBUG_H__
#ifdef _DEBUG
#include <cstdio>
#define log(args...) do { fprintf(stderr, args); fflush(stderr); } while(0)
#else
#define log(...)
#endif

#endif