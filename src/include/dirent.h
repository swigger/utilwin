#ifndef	_DIRENT_H
#define	_DIRENT_H
#include <sys/types.h>
#include "_muslfix.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __dirstream DIR;

#define _DIRENT_HAVE_D_RECLEN
#define _DIRENT_HAVE_D_OFF
#define _DIRENT_HAVE_D_TYPE

struct dirent {
	unsigned long long d_ino;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[256+5];
};

#define d_fileno d_ino

int            closedir(DIR *);
DIR           *fdopendir(HANDLE fd);
DIR           *opendir(const char *);
struct dirent *readdir(DIR *);
int            readdir_r(DIR *__restrict, struct dirent *__restrict, struct dirent **__restrict);
void           rewinddir(DIR *);
HANDLE           dirfd(DIR *);

int scandir(const char *, struct dirent ***, int (*)(const struct dirent *), int (*)(const struct dirent **, const struct dirent **));

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12
#define DT_WHT 14
#define IFTODT(x) ((x)>>12 & 017)
#define DTTOIF(x) ((x)<<12)
#endif


#ifdef __cplusplus
}
#endif

#endif
