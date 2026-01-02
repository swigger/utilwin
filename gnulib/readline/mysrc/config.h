#pragma once

#define HAVE_STDARG_H 1
#define HAVE_DIRENT_H 1
// #define HAVE_SYS_TIME_H 1
#define HAVE_TIMEVAL 1
#define __STDC__ 1 // define this for msvc.

#define HAVE_TERMIO_H 1


// these are all needed to make Multi Byte Support
#define HAVE_GETTIMEOFDAY 1
#define HAVE_WCTYPE_H 1
#define HAVE_WCHAR_H 1
#define HAVE_ISWCTYPE 1
#define HAVE_ISWLOWER 1
#define HAVE_ISWUPPER 1
#define HAVE_MBSRTOWCS 1
#define HAVE_MBRTOWC 1
#define HAVE_MBRLEN 1
#define HAVE_TOWLOWER 1
#define HAVE_TOWUPPER 1
#define HAVE_WCHAR_T 1
#define HAVE_WCWIDTH 1
#define HAVE_MBSTATE_T 1
#define WCHAR_T_BROKEN 1 //needed for 32bit wchar_t
typedef unsigned int char32_t;

#define _TANDEM_SOURCE 1
//#define HAVE_CHOWN 1
//#define HAVE_FCNTL 1
#define HAVE_FNMATCH 1
// #define HAVE_GETPWENT 1
//#define HAVE_GETPWNAM 1
//#define HAVE_GETPWUID 1
#define HAVE_KILL 1
#define HAVE_LSTAT 1
//#define HAVE_PSELECT 1
#define HAVE_PUTENV 1
#define HAVE_READLINK 1
// #define HAVE_SELECT 1
#define HAVE_SETENV 1
// #define HAVE_SETITIMER 1
#define HAVE_SETLOCALE 1
#define HAVE_STRCASECMP 1
#define HAVE_STRCOLL 1
#define HAVE_STRPBRK 1
#define HAVE_VSNPRINTF 1
#define HAVE_FCNTL_H 1
#define HAVE_LIBAUDIT_H 1
#define HAVE_LIMITS_H 1
#define HAVE_LOCALE_H 1
// #define HAVE_PWD_H 1
#define HAVE_STDBOOL_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
//#define HAVE_SYS_FILE_H 1
//#define HAVE_SYS_IOCTL_H 1
//#define HAVE_UNISTD_H 1
// #define HAVE_LANGINFO_CODESET 1
#define HAVE_DECL_AUDIT_USER_TTY 1
// #define GWINSZ_IN_SYS_IOCTL 1
// #define FIONREAD_IN_SYS_IOCTL 1
#define HAVE_GETPW_DECLS 1
//#define HAVE_POSIX_SIGNALS 1
//#define HAVE_POSIX_SIGSETJMP 1

#ifndef READLINE_LIBRARY
#define READLINE_LIBRARY 1
#endif

// avoid windows.h includes winsock.h
#define WIN32_LEAN_AND_MEAN 1
#undef UNICODE
#undef _UNICODE
#define _CRT_DECLARE_NONSTDC_NAMES 1
#define _AMD64_ 1
#include <minwindef.h>
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <stdlib.h>
// some one may forget to include stat.h
#include <sys/stat.h>
#include "_win.h"

#define stat _stat64
#define lstat _stat64
#define fstat _fstat64
#define S_ISLNK(m) 0
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define readlink wrl_readlink
#define alarm wrl_alarm
#define setenv wrl_setenv
#define getenv wrl_getenv
#define kill wrl_kill
#define ioctl wrl_ioctl
#define gettimeofday wrl_gettimeofday
#define wcwidth wrl_wcwidth
#define mbrtoc32 wrl_mbrtoc32
#define c32rtomb wrl_c32rtomb


typedef unsigned short mode_t;
typedef __int64 ssize_t;
typedef struct fd_set fd_set;
typedef struct { int a; } sigset_t;
struct timeval {
    long    tv_sec;         /* seconds */
    long    tv_usec;        /* and microseconds */
};
struct passwd
{
    char* pw_name;    /* Username.  */
    char* pw_passwd;  /* Hashed passphrase if Applicable */
    int pw_uid;       /* User ID.  */
    int pw_gid;       /* Group ID.  */
    char* pw_gecos;   /* Real name.  */
    char* pw_dir;     /* Home directory.  */
    char* pw_shell;   /* Shell program.  */
};

ssize_t readlink(const char* pathname, char* buf, size_t bufsiz);
unsigned int alarm(unsigned int seconds);
int setenv(const char* name, const char* value, int overwrite);
char* getenv(const char* name);
int kill(int pid, int sig);
int ioctl(int fd, unsigned long request, const void*);
int wcwidth(int c);
size_t mbrtoc32(char32_t* pc32,  const char* s, size_t n, mbstate_t* ps);
size_t c32rtomb(char* s, char32_t c32, mbstate_t* ps);
int gettimeofday(struct timeval* tv, struct timezone* tz);

#pragma warning(disable: 4267 4101 4005)
#pragma warning(disable: 4244) // conversion from 'int' to 'short', possible loss of data
#pragma warning(disable: 4102) // unreferenced label
