#include <Windows.h>
#include <stdint.h>
#include "unistd_uw.h"

#define DECL(type,var,base,off) type var = (type)((PCHAR)base + (INT_PTR)off)

int ReplaceIAT2(HMODULE mod, LPCSTR dname, void** func_from, void** func_to)
{
	void** oPos = 0;
	int rcnt = 0;
	if (!func_from || !mod) return 0;
	DECL(IMAGE_DOS_HEADER*, dosh, mod, 0);
	DECL(IMAGE_NT_HEADERS*, nth, mod, dosh->e_lfanew);
	DECL(PIMAGE_IMPORT_DESCRIPTOR, des, mod, nth->OptionalHeader.DataDirectory[1].VirtualAddress);
	DWORD sz = (nth->OptionalHeader.DataDirectory[1].Size) / sizeof(IMAGE_THUNK_DATA);
	DWORD oldpro;
	SIZE_T wtn;

	for (DWORD j = 0; j < sz && des[j].FirstThunk; ++j)
	{
		char* dname1 = (char*)mod + des[j].Name;
		if (dname && _stricmp(dname, dname1) != 0) continue;
		DECL(IMAGE_THUNK_DATA*, ith, mod, des[j].FirstThunk);

		for (int i = 0; ith[i].u1.Function; ++i)
		{
			oPos = (void**)&ith[i].u1.Function;
			// *oPos = tofunc;
			for (int k = 0; func_from[k]; ++k)
			{
				if (*oPos == func_from[k])
				{
					VirtualProtect(oPos, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldpro);
					WriteProcessMemory(GetCurrentProcess(), oPos, &func_to[k], sizeof(void*), &wtn);
					VirtualProtect(oPos, sizeof(void*), oldpro, &oldpro);
					++rcnt;
				}
			}
		}
	}
	return rcnt;
}
