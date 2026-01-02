#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int vasprintf(char** strp, const char* fmt, va_list ap)
{
	int q = _vscprintf(fmt, ap);
	if (!strp || q < 0) return q;
	*strp = (char*)malloc(q + 3);
	if (!*strp) return 0;
	_vsnprintf(*strp, q + 2, fmt, ap);
	(*strp)[q + 2] = 0;
	return q;
}

int asprintf(char ** strp, const char *fmt, ...)
{
	va_list ap;
	int res;
	va_start(ap, fmt);
	res = vasprintf(strp, fmt, ap);
	va_end(ap);
	return res;
}
