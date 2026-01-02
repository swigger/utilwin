#include <__build.h>
#include <dirent.h>
#include "__dirent.h"

void rewinddir(DIR *dir)
{
	AcquireSRWLockExclusive((PSRWLOCK)& dir->lock);
	dir->restart_ = 1;
	dir->ub_index = 0;
	ReleaseSRWLockExclusive((PSRWLOCK)& dir->lock);
}
