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

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <io.h>

#include "win32_u.h"
#include "win32u_int.h"

intptr_t ufprintf(FILE* fp, const char* fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	intptr_t nr = vufprintf(fp, fmt, vl);
	va_end(vl);
	return nr;
}

static bool IsCMDConsole(HANDLE hd)
{
	DWORD mode = 0;
	return !!GetConsoleMode(hd, &mode);
}

intptr_t vufprintf(FILE* fp, const char* fmt, va_list vl)
{
	if (!fp) return 0;

	size_t lenf = _vscprintf(fmt, vl);
	char * buf = (char*) malloc(lenf+1);
	_vsnprintf(buf, lenf+1, fmt, vl);
	buf[lenf] = 0;

	bool isConsole = IsCMDConsole((HANDLE) _get_osfhandle(_fileno(fp)));
	if (!isConsole || GetConsoleCP() == CP_UTF8 || is_7bits(buf, lenf))
	{
		int r = (int)fwrite(buf, 1, lenf, fp);
		free(buf);
		return r;
	}

	struct conv_buffer cb1, cb2;
	convbuf_init(&cb1);
	convbuf_init(&cb2);
	const wchar_t * s = convbuf_a2w(&cb1, buf, lenf);
	const char * s1 = convbuf_w2a(&cb2, s, -1, CP_ACP);
	intptr_t r = fwrite(s1, 1, strlen(s1), fp);
	convbuf_clear(&cb1);
	convbuf_clear(&cb2);
	free(buf);
	return r;
}

HANDLE WINAPI CreateFileU(
	_In_ const char* lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
)
{
	struct conv_buffer cb;
	convbuf_init(&cb);
	wchar_t * newfn = convbuf_a2w(&cb, lpFileName, -1);
	HANDLE r = CreateFileW(newfn,
		dwDesiredAccess, dwShareMode,
		lpSecurityAttributes, dwCreationDisposition,
		dwFlagsAndAttributes, hTemplateFile);
	convbuf_clear(&cb);
	return r;
}

BOOL WINAPI GetFileAttributesExU(
	_In_ const char* lpFileName,
	_In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
	_Out_writes_bytes_(sizeof(WIN32_FILE_ATTRIBUTE_DATA)) LPVOID lpFileInformation
)
{
	struct conv_buffer cb;
	convbuf_init(&cb);
	wchar_t * newfn = convbuf_a2w(&cb, lpFileName, -1);
	BOOL r = GetFileAttributesExW(newfn, fInfoLevelId, lpFileInformation);
	convbuf_clear(&cb);
	return r;
}

DWORD WINAPI GetFileAttributesU(
	_In_ const char* lpFileName
)
{
	struct conv_buffer cb;
	convbuf_init(&cb);
	wchar_t * newfn = convbuf_a2w(&cb, lpFileName, -1);
	DWORD r = GetFileAttributesW(newfn);
	convbuf_clear(&cb);
	return r;
}

BOOL WINAPI CopyFileU(
	_In_ const char* lpExistingFileName,
	_In_ const char* lpNewFileName,
	_In_ BOOL bFailIfExists
)
{
	struct conv_buffer cb1,cb2;
	convbuf_init(&cb1);
	convbuf_init(&cb2);
	wchar_t * na = convbuf_a2w(&cb1, lpExistingFileName, -1);
	wchar_t * nb = convbuf_a2w(&cb2, lpNewFileName, -1);
	BOOL r = CopyFileW(na, nb, bFailIfExists);
	convbuf_clear(&cb1);
	convbuf_clear(&cb2);
	return r;
}

BOOL WINAPI CreateDirectoryU(
	_In_ const char* lpPathName,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	struct conv_buffer cb;
	convbuf_init(&cb);
	wchar_t * newfn = convbuf_a2w(&cb, lpPathName, -1);
	BOOL r = CreateDirectoryW(newfn, lpSecurityAttributes);
	convbuf_clear(&cb);
	return r;
}

static bool w2u(const WIN32_FIND_DATAW* wfd, WIN32_FIND_DATAU* pf)
{
	int a = WideCharToMultiByte(CP_UTF8, 0, wfd->cAlternateFileName, -1,
		pf->cAlternateFileName, _countof(pf->cAlternateFileName), 0, 0);
	int b = WideCharToMultiByte(CP_UTF8, 0, wfd->cFileName, -1,
		pf->cFileName, _countof(pf->cFileName), 0, 0);
	pf->dwFileAttributes = wfd->dwFileAttributes;
	pf->ftCreationTime = ((uint64_t)wfd->ftCreationTime.dwHighDateTime << 32) + wfd->ftCreationTime.dwLowDateTime;
	pf->ftLastAccessTime = ((uint64_t)wfd->ftLastAccessTime.dwHighDateTime << 32) + wfd->ftLastAccessTime.dwLowDateTime;
	pf->ftLastWriteTime = ((uint64_t)wfd->ftLastWriteTime.dwHighDateTime << 32) + wfd->ftLastWriteTime.dwLowDateTime;
	pf->filesize = ((uint64_t)wfd->nFileSizeHigh << 32) + wfd->nFileSizeLow;
	pf->dwReserved0 = wfd->dwReserved0;
	pf->dwReserved1 = wfd->dwReserved1;
	return a >= 0 && b >= 0;
}

BOOL WINAPI FindNextFileU(_In_ HANDLE hFindFile, _Out_ WIN32_FIND_DATAU* pf)
{
	WIN32_FIND_DATAW wfd;
	if (FindNextFileW(hFindFile, &wfd))
	{
		w2u(&wfd, pf);
		return TRUE;
	}
	return FALSE;
}

HANDLE WINAPI FindFirstFileU(
	_In_ const char* lpFileName,
	_Out_ WIN32_FIND_DATAU* lpFindFileData
)
{
	struct conv_buffer cb;
	convbuf_init(&cb);
	wchar_t * newfn = convbuf_a2w(&cb, lpFileName, -1);
	WIN32_FIND_DATAW wfd;
	HANDLE hd = FindFirstFileW(newfn, &wfd);
	if (hd != INVALID_HANDLE_VALUE)
		w2u(&wfd, lpFindFileData);
	convbuf_clear(&cb);
	return hd;
}

DWORD WINAPI GetModuleFileNameU(HMODULE hModule, char* lpFilename, DWORD nSize)
{
	wchar_t wfn[400];
	wfn[0] = 0;
	DWORD xx = GetModuleFileNameW(hModule, wfn, _countof(wfn));
	if (xx == 0) return 0;

	struct conv_buffer cb;
	convbuf_init(&cb);
	char * newfn = convbuf_w2a(&cb, wfn, -1, CP_UTF8);
	strlcpy(lpFilename, newfn, nSize);
	DWORD r1 = (DWORD)strlen(newfn);
	convbuf_clear(&cb);
	return r1 < nSize ? r1 : nSize;
}
