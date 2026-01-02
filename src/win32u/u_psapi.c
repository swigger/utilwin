#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <Psapi.h>

#include "win32_u.h"
#include "win32u_int.h"

DWORD WINAPI GetModuleFileNameExU(HANDLE hProcess, HMODULE hModule, char* lpFilename, DWORD nSize)
{
	wchar_t wfn[400];
	DWORD xx =  GetModuleFileNameExW(hProcess, hModule, wfn, _countof(wfn));
	if (xx == 0) return 0;

	struct conv_buffer cb;
	convbuf_init(&cb);
	char * newfn = convbuf_w2a(&cb, wfn, -1, CP_UTF8);
	strlcpy(lpFilename, newfn, nSize);
	DWORD r1 = (DWORD)strlen(newfn);
	convbuf_clear(&cb);
	return r1 < nSize ? r1 : nSize;
}
