#include <__build.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "__dirent.h"

int readdir_r(DIR *restrict dir, struct dirent *restrict buf, struct dirent **restrict result)
{
	struct dirent *de;
	int errno_save = errno;
	int ret;

	AcquireSRWLockExclusive((PSRWLOCK)& dir->lock);
	errno = 0;
	de = readdir(dir);
	if ((ret = errno)) {
		ReleaseSRWLockExclusive((PSRWLOCK)& dir->lock);
		return ret;
	}
	errno = errno_save;
	if (de) memcpy(buf, de, de->d_reclen);
	else buf = NULL;

	ReleaseSRWLockExclusive((PSRWLOCK)& dir->lock);
	*result = buf;
	return 0;
}

LFS64_2(readdir_r, readdir64_r);
