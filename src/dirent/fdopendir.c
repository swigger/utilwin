#include "__build.h"
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include "__dirent.h"

DIR *fdopendir(HANDLE fd)
{
	DIR *dir;

	if (fd == 0 || fd == INVALID_HANDLE_VALUE) {
		return 0;
	}
	BY_HANDLE_FILE_INFORMATION xxx;
	if (!GetFileInformationByHandle(fd, &xxx)||
		xxx.dwFileAttributes == INVALID_FILE_ATTRIBUTES||
		(xxx.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0)
	{
		errno = ENOTDIR;
		return 0;
	}
	if (!(dir = calloc(1, sizeof *dir))) {
		return 0;
	}
	dir->fd = fd;
	dir->restart_ = 1;
	// DuplicateHandle(GetCurrentProcess(), fd, GetCurrentProcess(), &dir->fd, 0, FALSE, DUPLICATE_SAME_ACCESS);
	return dir;
}
