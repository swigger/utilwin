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

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vcruntime_startup.h>
#include <corecrt_startup.h>
#include <stdint.h>

#include "win32_u.h"
#include "win32u_int.h"


char** replace_args(int argc, char** argv)
{
	if (!__wargv)
		_configure_wide_argv(_get_startup_argv_mode());
	int nc = __argc;
	wchar_t** av = __wargv;
	int i;
	int needlen=0;
	(void)nc;

	for (i=0; i<argc; ++i)
		needlen += (int) wcslen(av[i]) + 1;
	wchar_t * allws = (wchar_t*) _alloca( needlen * sizeof(wchar_t) );
	wchar_t * pp = allws;
	for (i=0; i<argc; ++i)
	{
		size_t len_ = wcslen(av[i])+1;
		memcpy(pp, av[i], len_ * sizeof(wchar_t));
		pp += len_;
	}
	int olen = WideCharToMultiByte(CP_UTF8, 0, allws, needlen, 0, 0, 0,0);
	char * m_ = (char*)malloc(olen);
	char * pb = (char*)m_;
	WideCharToMultiByte(CP_UTF8, 0, allws, needlen, pb, olen, 0,0);
	for (i=0; i<argc; ++i)
	{
		argv[i] = pb;
		pb += strlen(pb)+1;
	}
	argv[argc] = 0;
	return argv;
}
