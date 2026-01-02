#include <__build.h>
#include <dirent.h>
#include "__dirent.h"

HANDLE dirfd(DIR *d)
{
	return d->fd;
}
