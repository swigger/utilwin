#include <tchar.h>
#include <Windows.h>
#include <locale.h>
#include <stdint.h>
#include <malloc.h>
#include <crtdbg.h>
#include <process.h>
#include <ndk/ntndk.h>
#include "unistd_uw.h"
#pragma section(".CRT$XIA5",    long, read) // First C Initializer
#define _CRTALLOC(x) __declspec(allocate(x))

static uint32_t* get_data_ptr(uint8_t* func)
{
	size_t n_next = 0;
	uint8_t* nextfunc[10];
	uint32_t flags = 0;
	for (;;)
	{
		int n = get_instruction_length(func, &flags);
#ifdef _M_IX86
		if (func[0] == 0xa1)
		{
			return *(uint32_t**)(func + 1);
		}
#else
		if (((flags & ILF_MODRM_MASK) >> ILF_MODRM_SHIFT) == 5)
		{
			uint8_t* p = func[0] == 0x48 ? func + 1 : func;
			if ((p[0] == 0x8b || p[0]==0x8d) && (p[1] & 0xc7) == 0x05)
			{
				return (uint32_t*)(p + 6 + *(int*)(p + 2));
			}
		}
#endif
		else if (func[0] == 0xe8)
		{
			uint8_t * f = func + 5 + (intptr_t) * (int*)(func + 1);
			if (n_next + 1 < _countof(nextfunc))
				nextfunc[n_next++] = f;
		}
		else if (func[0] == 0xc2 || func[0] == 0xc3)
		{
			if (n_next > 0)
			{
				--n_next;
				func = nextfunc[0];
				memmove(nextfunc, nextfunc + 1, n_next * sizeof(void*));
			}
			continue;
		}
		func += n;
	}
}

static void LdrpInitializeNlsInfo(PPEB a1)
{
	union {
		NLSTABLEINFO info;
		char buf[1024];
	}u;
	RtlInitNlsTables((PUSHORT)a1->AnsiCodePageData, (PUSHORT)a1->OemCodePageData, (PUSHORT)a1->UnicodeCaseTableData, &u.info);
	RtlResetRtlTranslations(&u.info);
}

static _Bool has_wide(char** str)
{
	int i, j;
	for (i = 0; str[i]; ++i)
	{
		char* p = str[i];
		for (j = 0; p[j]; ++j)
		{
			if ((p[j] >> 7) != 0)
				return TRUE;
		}
	}
	return FALSE;
}

static void* crt_malloc(size_t nlen)
{
#ifdef _DEBUG
	return _malloc_dbg(nlen, 2, __FILE__, __LINE__);
#else
	return malloc(nlen);
#endif
}

static int early_patch_utf8()
{
#ifdef _M_IX86
	PPEB peb = (PPEB)__readfsdword(0x30);
#else
	PPEB peb = (PPEB)__readgsqword(0x60);
#endif
	peb->AnsiCodePageData = 0;
	peb->OemCodePageData = 0;
	LdrpInitializeNlsInfo(peb);
	HMODULE mod = GetModuleHandleA("KernelBase.dll");
	if (!mod) mod = GetModuleHandleA("Kernel32.dll");
	get_data_ptr((uint8_t*)GetProcAddress(mod, "GetACP"))[0] = CP_UTF8;
	get_data_ptr((uint8_t*)GetProcAddress(mod, "GetOEMCP"))[0] = CP_UTF8;
	// replace command linew
	wchar_t* cmdw = peb->ProcessParameters->CommandLine.Buffer;
	for (int i = 0; cmdw[i]; ++i)
	{
		if ((uint16_t)cmdw[i] >= 0x80)
		{
			ANSI_STRING ansi = {0};
			RtlUnicodeStringToAnsiString(&ansi, &peb->ProcessParameters->CommandLine, 1);
			LPCSTR* px = (LPCSTR*)get_data_ptr((uint8_t*)GetProcAddress(mod, "GetCommandLineA"));
			*px = ansi.Buffer;
			break;
		}
	}

#ifdef _acmdln
	_acmdln = GetCommandLineA();
	char *** env1 = &_environ;
	if (env1 && *env1 && **env1 && has_wide(*env1))
	{
		LPWSTR const w = GetEnvironmentStringsW();
		LPCWSTR p = w;
		int i = 0;
		while (*p) {
			if (*p != '=') ++i;
			p += wcslen(p) + 1;
		}
		int ncch = (int)(intptr_t)(p + 1 - w);
		int need = WideCharToMultiByte(CP_UTF8, 0, w, ncch, 0, 0, 0, 0);
		char* buf1 = (char*)malloc(need);
		WideCharToMultiByte(CP_UTF8, 0, w, ncch, buf1, need, 0, 0);
		FreeEnvironmentStringsW(w);
		char** tenv = (char**)crt_malloc(sizeof(void*)*(i+1));
		char** const tenv1 = tenv;
		for (char * p1=buf1; *p1; )
		{
			size_t nlen = strlen(p1) + 1;
			if (*p1 != '=')
				*tenv++ = (char*)memcpy(crt_malloc(nlen), p1, nlen);
			p1 += nlen;
		}
		*tenv = 0;
		free(buf1);
		env1[0] = tenv1;
		env1[1] = tenv1;
		BYTE* func = function_start(_get_initial_narrow_environment);
		*(char***)get_data_ptr(func) = tenv1;
	}
#endif
	return 0;
}

_CRTALLOC(".CRT$XIA5") void* call_early_patch_utf8 = &early_patch_utf8;

void uw_enable_utf8()
{
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, "en_US.UTF-8");
	void* volatile val = &call_early_patch_utf8;
	(void)val;
}

/*
argv is set in pre_c_initialization. So early reset GetCommandLine can take effect on it.
ucrtbased._get_wpgmptr+434
ucrtbased._configure_narrow_argv+11
qapp1.__scrt_narrow_argv_policy::configure_argv+10
qapp1.pre_c_initialization+44
ucrtbased._initterm_e+63
qapp1.__scrt_common_main_seh+68
qapp1.__scrt_common_main+E
qapp1.mainCRTStartup+E

env is inited by some dependency dll before mainCRTStartup. so we must replace it.
	ucrtbased.dll!common_get_or_create_environment_nolock<char>() Line 285	C++
	ucrtbased.dll!getenv_s(unsigned __int64 * required_count, char * buffer, unsigned __int64 buffer_count, const char * name) Line 175	C++
	Qt5Cored.dll!qEnvironmentVariableIsSet() + 0x38 bytes	Unknown
	ucrtbased.dll!_initterm(void(*)() * first, void(*)() * last) Line 22	C++
	ntdll.dll!LdrpInitialize()	Unknown
*/
