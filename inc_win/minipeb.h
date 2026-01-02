#pragma once

#include <intrin.h>

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _PEB_LDR_DATA
{
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB
{
	BOOLEAN InheritedAddressSpace;
	BOOLEAN ReadImageFileExecOptions;
	BOOLEAN BeingDebugged;
	BOOLEAN SpareBool;
	HANDLE Mutant;
	PVOID ImageBaseAddress;
	PPEB_LDR_DATA Ldr;
	PVOID ProcessParameters; //PRTL_USER_PROCESS_PARAMETERS
	PVOID SubSystemData;
	PVOID ProcessHeap;
	struct _RTL_CRITICAL_SECTION* FastPebLock;
}PEB, *PPEB;

typedef struct _TEB
{
	NT_TIB         NtTib;
	PVOID             EnvironmentPointer;
	CLIENT_ID      ClientId;
	PVOID             ActiveRpcHandle;
	PVOID             ThreadLocalStoragePointer;
	PPEB      ProcessEnvironmentBlock;
}TEB, *PTEB;

#ifdef __cplusplus
extern "C"{
#endif

	extern PPEB WINAPI RtlGetCurrentPeb();

#ifdef __cplusplus
}
#endif

#pragma comment(lib, "ntdll.lib")
