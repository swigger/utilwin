#include <memory.h>
#include <string.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
extern void* xmalloc(size_t n);

char* xbfd_getpwd() {
	char buf[512];
	DWORD len = GetCurrentDirectoryA(sizeof(buf), buf);
	if (len == 0 || len >= sizeof(buf)) {
		return 0;
	}
	char* r = (char*)xmalloc(len + 1);
	memcpy(r, buf, len);
	r[len] = 0;
	return r;
}
