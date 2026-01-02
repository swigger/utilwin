#pragma once
#define WIN32_NO_STATUS
#include <Windows.h>
#include <ndk/ntndk.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
void __seterrno_from_nt_status(NTSTATUS status);
