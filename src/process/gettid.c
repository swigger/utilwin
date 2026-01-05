#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "unistd_uw.h"

pid_t gettid()
{
	return GetCurrentThreadId();
}
