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

#include <__build.h>
#include <__ntenv.h>
#define _GNU_SOURCE 1
#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <assert.h>
#include "__dirent.h"

struct dirent *readdir(DIR *dir)
{
	struct dirent *de;
	if (dir->ub_len <= dir->ub_index || dir->restart_)
	{
		IO_STATUS_BLOCK iob = { 0 };
		NTSTATUS nth = NtQueryDirectoryFile(dir->fd, /*event=*/NULL,  /*APC=*/NULL, /*apc_ctx=*/NULL, &iob,
				dir->uni_buffer, sizeof(dir->uni_buffer), FileIdBothDirectoryInformation, /*single_entry=*/FALSE, /*fn_glob=*/NULL,
				dir->restart_
				);
		if (nth == STATUS_NO_MORE_FILES)
		{
			errno = 0;
			return 0;
		}
		if (FAILED(nth))
		{
			__seterrno_from_nt_status(nth);
			return 0;
		}
		dir->restart_ = 0;
		dir->ub_len = (unsigned short) iob.Information;
		dir->ub_index = 0;
	}
	FILE_ID_BOTH_DIR_INFORMATION * fdi = (FILE_ID_BOTH_DIR_INFORMATION*)(dir->uni_buffer + dir->ub_index);
	de = (struct dirent *)(dir->abuf);
	de->d_ino = fdi->FileId.QuadPart;
	de->d_type = (fdi->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? DT_DIR : DT_REG;
	if (fdi->FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) de->d_type = DT_LNK;
	int q = WideCharToMultiByte(CP_UTF8, 0, fdi->FileName, fdi->FileNameLength/2, de->d_name, sizeof(dir->abuf) - 1 - offsetof(struct dirent,d_name), 0, 0);
	assert(q > 0);
	de->d_name[q] = 0;
	//de->d_namlen = q;
	de->d_reclen = (unsigned short)(sizeof(struct dirent) - sizeof(de->d_name) + q+1);
	de->d_reclen = (de->d_reclen + 7) & ~7;
	dir->ub_index = (unsigned short)((char*)fdi + fdi->NextEntryOffset - dir->uni_buffer);
	if (fdi->NextEntryOffset == 0)
	{
		dir->ub_index = dir->ub_len;
	}
	return de;
}
