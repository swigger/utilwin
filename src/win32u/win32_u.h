#pragma once

#ifdef _WIN32

#ifdef __cplusplus
extern "C" {
#endif
	char** replace_args(int argc, char** argv);
	intptr_t ufprintf(FILE* fp, const char* fmt, ...);
	intptr_t vufprintf(FILE* fp, const char* fmt, va_list vl);

	HANDLE WINAPI CreateFileU(
		_In_ const char* lpFileName,
		_In_ DWORD dwDesiredAccess,
		_In_ DWORD dwShareMode,
		_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		_In_ DWORD dwCreationDisposition,
		_In_ DWORD dwFlagsAndAttributes,
		_In_opt_ HANDLE hTemplateFile
	);

	BOOL WINAPI GetFileAttributesExU(
		_In_ const char* lpFileName,
		_In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
		_Out_writes_bytes_(sizeof(WIN32_FILE_ATTRIBUTE_DATA)) LPVOID lpFileInformation
	);
	DWORD WINAPI GetFileAttributesU(
			_In_ const char* lpFileName
		);
	BOOL WINAPI CopyFileU(
			_In_ const char* lpExistingFileName,
			_In_ const char* lpNewFileName,
			_In_ BOOL bFailIfExists
		);

	BOOL WINAPI CopyFileU(
			_In_ const char* lpExistingFileName,
			_In_ const char* lpNewFileName,
			_In_ BOOL bFailIfExists
		);

	BOOL WINAPI CreateDirectoryU(
			_In_ const char* lpPathName,
			_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes
		);

	typedef struct _WIN32_FIND_DATAU {
		DWORD dwFileAttributes;
		uint64_t ftCreationTime;
		uint64_t ftLastAccessTime;
		uint64_t ftLastWriteTime;
		uint64_t filesize;
		DWORD dwReserved0;
		DWORD dwReserved1;
		char cAlternateFileName[44];
		char cFileName[784];
	} WIN32_FIND_DATAU;

	BOOL WINAPI FindNextFileU(
			_In_ HANDLE hFindFile,
			_Out_ WIN32_FIND_DATAU* lpFindFileData
		);
	HANDLE WINAPI FindFirstFileU(
			_In_ const char* lpFileName,
			_Out_ WIN32_FIND_DATAU* lpFindFileData
		);
	DWORD WINAPI GetModuleFileNameU(HMODULE hModule,  char* lpFilename, DWORD nSize);
	DWORD WINAPI GetModuleFileNameExU(HANDLE hProcess, HMODULE hModule, char* lpFilename, DWORD nSize);

#ifdef  __cplusplus
}
#endif

#else
#define ufprintf fprintf
#define vufprintf vfprintf
inline char** replace_args(int argc, const char** argv) {return 0;}
#endif
