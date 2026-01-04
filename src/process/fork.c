/*
 * Copyright 2019-2026 lixungeng
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <__ntenv.h>
#include <stdio.h>
#include <stdint.h>
#include <setjmp.h>
#include <io.h>
#include <assert.h>
#include "unistd_uw.h"

#define RTL_CLONE_PARENT                                0
#define RTL_CLONE_CHILD                                 297

#define VAL(type, mod, addr) (*(type*)((char*)mod+addr))

#pragma region DllData
struct data_block_t {
	void* modbase;
	uint32_t offset;
	uint32_t size;
	char data[0];
};

struct data_block_t *g_ntdll, *g_kernelbase, *g_kernel32;

static int prepare_dll(const char* name, struct data_block_t** optr) {
	if (*optr) return 0;
	wchar_t path[300];
	HMODULE mod = GetModuleHandleA(name);
	GetModuleFileNameW(mod, path, _countof(path));

	PIMAGE_DOS_HEADER dosh = (IMAGE_DOS_HEADER*)mod;
	PIMAGE_NT_HEADERS nth = (IMAGE_NT_HEADERS*)((char*)mod + dosh->e_lfanew);
	PIMAGE_SECTION_HEADER secs = IMAGE_FIRST_SECTION(nth);
	PIMAGE_SECTION_HEADER datah = NULL;
	for (int i = 0; i < nth->FileHeader.NumberOfSections; ++i) {
		if (strcmp((char*)secs[i].Name, ".data") == 0) {
			datah = &secs[i];
			break;
		}
	}
	if (!datah) return -1;
	struct data_block_t* op = (struct data_block_t*) HeapAlloc(GetProcessHeap(), 0, datah->Misc.VirtualSize + sizeof(struct data_block_t));
	if (!op) return -1;
	op->offset = datah->VirtualAddress;
	op->size = datah->Misc.VirtualSize;
	memset(&op->data[0], 0, op->size);
	HANDLE hf = CreateFileW(path, FILE_READ_DATA, FILE_SHARE_READ|FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
	if (hf == INVALID_HANDLE_VALUE) {
		HeapFree(GetProcessHeap(), 0, op);
		return -1;
	}
	SetFilePointer(hf, datah->PointerToRawData, NULL, FILE_BEGIN);
	DWORD rlen;
	DWORD to_read = min(datah->SizeOfRawData, datah->Misc.VirtualSize);
	if (!ReadFile(hf, &op->data[0], to_read, &rlen, NULL)) {
		CloseHandle(hf);
		HeapFree(GetProcessHeap(), 0, op);
		return -1;
	}
	CloseHandle(hf);
	for (DWORD off = 0; off < to_read; off += sizeof(void*)) {
		uintptr_t* dest = (uintptr_t*)&op->data[off];
		uintptr_t* src = (uintptr_t*)((char*)mod + off + op->offset);
		if (*dest == 0) continue;
		if ((*dest & 0xffff) == (*src & 0xffff)) {
			*dest = *src;
		}
	}
	op->modbase = mod;
	*optr = op;
	return 0;
}

static BOOL copy_memory(HANDLE hpro, struct data_block_t* d) {
	SIZE_T wtn;
	return !!WriteProcessMemory(hpro, (char*)d->modbase + d->offset, &d->data[0], d->size, &wtn);
}

struct ntptrs_t {
	void** CsrPortHandle;
	void** LdrpMrdataLock;
};
struct ntptrs_t* g_ntptrs = 0;
BOOL init_ntptrs() {
	HMODULE ntdll = GetModuleHandleA("ntdll.dll");
	if (!ntdll) return 0;
	// find the two critical variable.
	uint8_t* f1 = (uint8_t*)GetProcAddress(ntdll, "CsrClientConnectToServer");
	uint8_t* f2 = (uint8_t*)GetProcAddress(ntdll, "RtlGrowFunctionTable");
	uint32_t i, flags;
	intptr_t v1 = 0, v2 = 0;
	for (i = 0; i < 300; ++i)
	{
		// CsrClientConnectToServer:  cmp     cs:CsrPortHandle, rax
		int len = get_instruction_length(f1, &flags);
		if (f1[flags & ILF_PLENMASK] == 0x39 && len > 4) //cmp m64,reg
			v1 = (intptr_t)(f1 + len + *(int*)(f1 + len - 4));
		if (f1[0] == 0xc2 || f1[0] == 0xc3)
			break;
		f1 += len;
	}
	for (i = 0; i < 300; ++i)
	{
		// RtlGrowFunctionTable ldr reg, addr // LdrpMrdataLock
		int len = get_instruction_length(f2, &flags);
		if (f2[flags & ILF_PLENMASK] == 0x8d && len > 4) //lea reg,m64
			v2 = (intptr_t)(f2 + len + *(int*)(f2 + len - 4));
		if (f2[0] == 0xc2 || f2[0] == 0xc3)
			break;
		f2 += len;
	}
	g_ntptrs = (struct ntptrs_t*) HeapAlloc(GetProcessHeap(), 0, sizeof(struct ntptrs_t));
	g_ntptrs->CsrPortHandle = &VAL(void*, ntdll, v1 - (intptr_t)ntdll);// 0x16CC48
	g_ntptrs->LdrpMrdataLock = &VAL(void*, ntdll, v2 - (intptr_t)ntdll);// 0x16c110
	return TRUE;
}
#pragma endregion

#pragma region evil_funcs
struct xxval_t
{
	char unk[0x58];
	HANDLE handle;
};
static void clean_etw_obj(intptr_t obj)
{
	char buf[2048];
	ULONG rlen;
	struct xxval_t* xx = (struct xxval_t*)(obj & 0xffffFFFFffff);
	if (xx && xx->handle != INVALID_HANDLE_VALUE && xx->handle)
	{
		// is it a valid handle?
		NTSTATUS st = ZwQueryObject(xx->handle, ObjectTypeInformation, &buf[0], sizeof(buf), &rlen);
		OBJECT_TYPE_INFORMATION* ti = (OBJECT_TYPE_INFORMATION*)buf;
		LPCWSTR name = ti->TypeName.Buffer;
		(void)name;
		if (st < 0)
			xx->handle = INVALID_HANDLE_VALUE;
	}
}

static int (*old_EtwEventUnregister)(intptr_t obj);
static int my_EtwEventUnregister(intptr_t obj)
{
	// clean_etw_obj(obj); return old_EtwEventUnregister(obj);
	return 1;
}
static int (*old_EtwUnregisterTraceGuids)(intptr_t obj);
static int my_EtwUnregisterTraceGuids(intptr_t obj)
{
	// clean_etw_obj(obj); return old_EtwUnregisterTraceGuids(obj);
	return 1;
}

static void replace_evil_funcs(HMODULE ntdll)
{
	void* func_from[] = {
	(void*)GetProcAddress(ntdll, "EtwEventUnregister"),
	(void*)GetProcAddress(ntdll, "EtwUnregisterTraceGuids"),
	NULL
	};
	void* func_to[] = {
		(void*)my_EtwEventUnregister,
		(void*)my_EtwUnregisterTraceGuids,
		0
	};
	*(void**)&old_EtwEventUnregister = func_from[0];
	*(void**)&old_EtwUnregisterTraceGuids = func_from[1];

	LIST_ENTRY* le = &NtCurrentPeb()->Ldr->InLoadOrderModuleList;
	LIST_ENTRY* p;
	// ignore first one(the exe) and second one(ntdll.dll)
	for (p = le->Flink->Flink->Flink; p != le; p = p->Flink)
	{
		LDR_DATA_TABLE_ENTRY* dt = (LDR_DATA_TABLE_ENTRY*)p;
		ReplaceIAT2((HMODULE)dt->DllBase, NULL, func_from, func_to);
	}
}
#pragma endregion

static BOOL ReInit(const char* modname, int modid)
{
	int (WINAPI * dllmain_)(HMODULE, DWORD, void*);
	char* mod = (char*)GetModuleHandleA(modname);
	if (!mod) return FALSE;
	PIMAGE_DOS_HEADER dosh = (PIMAGE_DOS_HEADER)mod;
	PIMAGE_NT_HEADERS nth = (PIMAGE_NT_HEADERS)(mod + dosh->e_lfanew);
	DWORD entry = nth->OptionalHeader.AddressOfEntryPoint;
	if (!entry) return TRUE;

	*(void**)&dllmain_ = mod + entry;
	return dllmain_((HMODULE)mod, 1, 0);
}

static int child_entry(void)
{
	*g_ntptrs->CsrPortHandle = 0;
	*g_ntptrs->LdrpMrdataLock = 0;
	NtCurrentPeb()->ProcessParameters->ConsoleHandle = INVALID_HANDLE_VALUE;
	ReInit("KernelBase.dll", 1);
	ReInit("Kernel32.dll", 2);
	// 不需要 FreeConsole，本来就没有...
	// FreeConsole();

#if 0  // user32想都不要想，不可能的...
	{
		char ch = 0;
		SIZE_T nw = 0;
		DWORD oldpro;
		VAR(uint8_t, pfn_inited, "ntdll.dll", 0x1811D8);
		VirtualProtect(ppfn_inited, 1, PAGE_READWRITE, &oldpro);
		WriteProcessMemory(GetCurrentProcess(), ppfn_inited, &ch, 1, &nw);
		VirtualProtect(ppfn_inited, 1, oldpro, &oldpro);
		ReInit("user32.dll");
	}
#endif
	// reopen stdio.
	(void)freopen("nul", "r", stdin);
	(void)freopen("nul", "w", stdout);
	(void)freopen("nul", "w", stderr);

	//TODO: old version may need this.
	// replace_evil_funcs(GetModuleHandleA("ntdll"));
	return 0;
}

typedef struct mem_range_t_ {
	uintptr_t start;
	uintptr_t end;
}mem_range_t;

static BOOL in_range(uintptr_t ptr, mem_range_t * rngs, size_t nrng) {
	size_t i;
	for (i = 0; i < nrng; ++i) {
		if (ptr >= rngs[i].start && ptr < rngs[i].end) return TRUE;
	}
	return FALSE;
}

static void CopyMemoryTo(HANDLE hChild) {
	// the buggy fork won't copy some memory to child, let's do it by hand!
	HMODULE mod = GetModuleHandleA("ntdll.dll");
	IMAGE_DOS_HEADER * dosh = (IMAGE_DOS_HEADER*)mod;
	IMAGE_NT_HEADERS * nth = (IMAGE_NT_HEADERS*)((char*)mod + dosh->e_lfanew);
	IMAGE_SECTION_HEADER * sec = (IMAGE_SECTION_HEADER*)((char*)&nth->OptionalHeader + nth->FileHeader.SizeOfOptionalHeader);
	IMAGE_SECTION_HEADER* data_sec = 0;
	DWORD i;
	// find .data section.
	for (i = 0; i < nth->FileHeader.NumberOfSections; ++i)
	{
		if (strcmp((char*)sec[i].Name, ".data") == 0)
		{
			data_sec = &sec[i];
			break;
		}
	}

	DWORD size = nth->OptionalHeader.SizeOfImage;
	size = (size + 0xfff) & ~0xfff;
	mem_range_t ranges[1024]; // hope it's enough.
	uint32_t range_count = 0;
	ranges[range_count].start = (uintptr_t)mod;
	ranges[range_count].end = (uintptr_t)mod + size;
	++range_count;

	for (i=0; i<data_sec->Misc.VirtualSize; i+=8){
		uintptr_t addr = (uintptr_t)mod + data_sec->VirtualAddress + i;
		uintptr_t val = *(uintptr_t*)addr;
		if (val <= 0xffff) continue;
#ifdef _WIN64
		if (val >> 47) continue;
#else
		if (val >> 31) continue;
#endif
		val -= val % PAGE_SIZE;
		if (in_range(val, ranges, range_count)) continue;

		MEMORY_BASIC_INFORMATION mbi;
		memset(&mbi, 0, sizeof(mbi));
		if (IsBadReadPtr((void*)val, 8)) {
			if (VirtualQuery((void*)val, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
			{
				// mark the hole
				ranges[range_count].start = (uintptr_t)mbi.BaseAddress;
				ranges[range_count].end = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
				++range_count;
			}
			continue;
		}
		uintptr_t test_ptr = 0;
		// this block of memory may not copied
		if (!ReadProcessMemory(hChild, (void*)val, &test_ptr, 8, NULL)) {
			if (VirtualQuery((void*)val, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
			{
				VirtualAllocEx(hChild, mbi.AllocationBase,
					(uintptr_t)mbi.BaseAddress- (uintptr_t)mbi.AllocationBase+mbi.RegionSize,
					MEM_RESERVE, PAGE_READWRITE);
				VirtualAllocEx(hChild, mbi.BaseAddress, mbi.RegionSize,	MEM_COMMIT, PAGE_READWRITE);
				WriteProcessMemory(hChild, (void*)mbi.BaseAddress, mbi.BaseAddress, mbi.RegionSize, NULL);
				ranges[range_count].start = (uintptr_t)mbi.BaseAddress;
				ranges[range_count].end = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
				++range_count;
			}
		}
	}
	// DONE.
	assert(range_count < _countof(ranges));
}

int fork(void)
{
	uintptr_t h;
	HANDLE console_handle = NtCurrentPeb()->ProcessParameters->ConsoleHandle;
	NTSTATUS st;
	RTL_USER_PROCESS_INFORMATION process_info = {sizeof(process_info)};
	TCHAR cwd[MAX_PATH] = {0};
	if (!g_ntptrs) init_ntptrs();
	prepare_dll("ntdll.dll", &g_ntdll);
	prepare_dll("kernelbase.dll", &g_kernelbase);
	prepare_dll("kernel32.dll", &g_kernel32);

	for (h = 4; h <= 0x2000; h += 4)
	{
		if ((HANDLE)h == console_handle) continue;
		char buf[2048];
		ULONG rlen = 0;
		st = ZwQueryObject((HANDLE)h, ObjectNameInformation, &buf[0], sizeof(buf), &rlen);
		if (st < 0) continue;
		LPCWSTR sname = ((UNICODE_STRING*)buf)->Buffer;
		if (!sname)
		{
			ZwQueryObject((HANDLE)h, ObjectTypeInformation, &buf[0], sizeof(buf), &rlen);
			OBJECT_TYPE_INFORMATION* ti = (OBJECT_TYPE_INFORMATION*)buf;
			if (ti->TypeName.Buffer) sname = ti->TypeName.Buffer;
		}
		if (sname)
		{
			// (Process and Thread)es should be set inherit by application. not fork.
			// Console is set later below. don't inherit console reference. we'd detach.
			BOOL skip = _wcsnicmp(sname, L"\\Device\\ConDrv", 14) == 0 ||
				wcscmp(sname, L"Process") == 0 || wcscmp(sname, L"Thread") == 0;
			// fprintf(stderr, "%llx %S%s\n", h + 0llu, sname, skip?" SKIPed":"");
			if (skip) continue;
		}
		SetHandleInformation((HANDLE)h, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
	}

	SetHandleInformation(GetStdHandle(STD_INPUT_HANDLE), HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
	SetHandleInformation(GetStdHandle(STD_OUTPUT_HANDLE), HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
	SetHandleInformation(GetStdHandle(STD_ERROR_HANDLE), HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
	GetCurrentDirectory(MAX_PATH, cwd);

	process_info.Length = sizeof(process_info);
	st = RtlCloneUserProcess(RTL_CLONE_PROCESS_FLAGS_CREATE_SUSPENDED | RTL_CLONE_PROCESS_FLAGS_INHERIT_HANDLES, NULL, NULL, NULL, &process_info);
	if (st == RTL_CLONE_PARENT)
	{
		// TODO: try to copy user32!gSharedInfo to child process, to let messagebox run? may impossible.
		// copy_memory(process_info.ProcessHandle, g_ntdll);
		copy_memory(process_info.ProcessHandle, g_kernelbase);
		copy_memory(process_info.ProcessHandle, g_kernel32);
		CopyMemoryTo(process_info.ProcessHandle);
		ResumeThread(process_info.ThreadHandle);
		CloseHandle(process_info.ProcessHandle);
		CloseHandle(process_info.ThreadHandle);
		return (int)(intptr_t)process_info.ClientId.UniqueProcess;
	}
	else if (st == RTL_CLONE_CHILD)
	{
		int rt = child_entry();
		SetCurrentDirectory(cwd);
		return rt;
	}
	else
	{
		SetLastError(RtlNtStatusToDosError(st));
		return -1;
	}
}

#if 0  // this code was used to research the evail handle mechanism.
namespace clean_evil_handles
{
	struct packaged_handler {
		uint64_t handle : 48;
		uint64_t flags : 16;
	};

	struct xxval_t
	{
		char unk[0x58];
		HANDLE handle;
	};

	struct alist
	{
		alist* next;
		packaged_handler ph;
	};
	void set_handler(char* dll, uint32_t offset, HANDLE val = INVALID_HANDLE_VALUE)
	{
		if (!dll) return;
		intptr_t x = *(intptr_t*)(dll + offset);
		x = x & 0xffffFFFFffff;
		if (!x) return;
		*(HANDLE*)(x + 0x58) = val;
	}

	void set_list_handler(char* dll, uint32_t offset, HANDLE val = INVALID_HANDLE_VALUE)
	{
		if (!dll) return;
		alist* lend = (alist*)(dll + offset);
		for (auto l = lend->next; l && l != lend; l = l->next)
		{
			auto p = (xxval_t*)l->ph.handle;
			p->handle = val;
		}
	}

	void doit()
	{
		auto dll = (char*)GetModuleHandleA("sechost.dll");
		set_handler(dll, 0x8E798);
		set_handler(dll, 0x8E740);
		set_handler(dll, 0x8e760);
		dll = (char*)GetModuleHandleA("uxtheme.dll");
		set_handler(dll, 0x91320);
		dll = (char*)GetModuleHandleA("ole32.dll");
		set_handler(dll, 0xff0e8 + 0x20);
		set_handler(dll, 0xff0d0);
		set_list_handler(dll, 0xff000);
		dll = (char*)GetModuleHandleA("combase.dll");
		set_handler(dll, 0x300480);
		set_handler(dll, 0x300430);
		set_handler(dll, 0x3003e0);
		set_handler(dll, 0x300520);
		set_handler(dll, 0x3012e8 + 0x20);
		set_handler(dll, 0x303ee0);
		set_handler(dll, 0x3012a0 + 0x20);
		set_handler(dll, 0x301f20 + 0x20);
		set_handler(dll, 0x301390 + 0x20);
		set_list_handler(dll, 0x3000b0);
		dll = (char*)GetModuleHandleA("user32.dll");
		set_handler(dll, 0xb2898);
		set_handler(dll, 0xb2728);
		set_handler(dll, 0xb26f0);
		dll = (char*)GetModuleHandleA("ws2_32.dll");
		set_list_handler(dll, 0x52008);
		set_handler(dll, 0x52858);
		set_handler(dll, 0x52030);
	}
}
#endif
