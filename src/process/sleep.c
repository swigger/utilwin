#include <Windows.h>
#include <winternl.h>
#include <stdint.h>
#include <errno.h>
#include "unistd_uw.h"

#ifndef STATUS_ALERTED
#define STATUS_ALERTED 0x101
#endif

NTSYSAPI NTSTATUS NTAPI NtDelayExecution(BOOLEAN Alertable, PLARGE_INTEGER DelayInterval);

static int conv_ntstatus(NTSTATUS nt)
{
	if (nt == STATUS_ALERTED || nt == STATUS_USER_APC)
	{
		errno = EINTR;
		return -1;
	}
	return 0;
}

int usleep(uint32_t timeout)
{
	LARGE_INTEGER li;
	li.QuadPart = -(int64_t)timeout * 10;
	NTSTATUS nt = NtDelayExecution(FALSE, (timeout == -1) ? 0 : &li);
	return conv_ntstatus(nt);
}

int sleep(int s)
{
	LARGE_INTEGER delay;
	delay.QuadPart = -(__int64)s * 10000000;
	NTSTATUS nt = NtDelayExecution(TRUE, (s == -1) ? 0 : &delay);
	return conv_ntstatus(nt);
}

int sched_yield(void)
{
	LARGE_INTEGER delay;
	delay.QuadPart = 0;
	NTSTATUS nt = NtDelayExecution(TRUE, &delay);
	return conv_ntstatus(nt);
}

pid_t gettid()
{
	return GetCurrentThreadId();
}
