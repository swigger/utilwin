#pragma once
#define WIN32_NO_STATUS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>

void __seterrno_from_nt_status(long/*NTSTATUS*/ status);
