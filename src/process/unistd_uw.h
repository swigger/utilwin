#pragma once
#include "disasmlen.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef int pid_t;

	void uw_enable_utf8(void* unused UUW_DEFAULT_PARAM(0));
	void prehook(void);
	void* _nothing_func(const void* arg);
	int ReplaceIAT2(HMODULE mod, LPCSTR dname, void** func_from, void** func_to);
	int socketpair(int family, int type, int protocol, uintptr_t fd[2]);

	int fork(void);
	int sleep(int s);
	int usleep(uint32_t s);
	pid_t gettid();
	int sched_yield(void);

	// should not here. but nowhere to place
	int asprintf(char **strp, const char *fmt, ...);
	int vasprintf(char **strp, const char *fmt, va_list ap);

#ifdef __cplusplus
}
#endif
