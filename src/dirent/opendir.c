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

#define _GNU_SOURCE
#include <__build.h>
#include <__ntenv.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include "__dirent.h"

static HANDLE opendir_hd(const char * name)
{
	int uname = MultiByteToWideChar(CP_UTF8, 0, name, -1, 0, 0) + 6;
	wchar_t * res = (wchar_t*)malloc(sizeof(wchar_t)*uname);
	MultiByteToWideChar(CP_UTF8, 0, name, -1, res, uname);
	UNICODE_STRING upath;
	ULONG file_attributes = 0;
	ULONG shared = FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE;
	ULONG create_disposition = FILE_OPEN;
	ULONG options = FILE_OPEN_FOR_BACKUP_INTENT|FILE_SYNCHRONOUS_IO_NONALERT;
	OBJECT_ATTRIBUTES attr;
	IO_STATUS_BLOCK io;
	NTSTATUS status;
	HANDLE fh = INVALID_HANDLE_VALUE;

	RtlDosPathNameToNtPathName_U(res, &upath, 0, 0);
	InitializeObjectAttributes(&attr, &upath, 0, NULL, 0);
	status = NtCreateFile(&fh, GENERIC_READ | SYNCHRONIZE, &attr, &io, NULL, file_attributes, shared,
			        create_disposition, options, NULL, 0);
	free(res);
	RtlFreeUnicodeString(&upath);

	if (!NT_SUCCESS(status))
		return INVALID_HANDLE_VALUE;
	return fh;
}

DIR *opendir(const char *name)
{
	HANDLE fd;
	DIR *dir;

	fd = opendir_hd(name);
	if (fd == INVALID_HANDLE_VALUE)
		return 0;
	if (!(dir = calloc(1, sizeof *dir))) {
		CloseHandle(fd);
		return 0;
	}
	dir->fd = fd;
	return dir;
}
