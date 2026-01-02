#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "win32_u.h"
#include "win32u_int.h"

enum {
	DEFARRSZ = sizeof( ((struct conv_buffer*)0x10)->buf ),
	HDR_SIZE = offsetof(struct conv_buffer, buf),
	SP_WSZ = DEFARRSZ/3-1,
	SP_ASZ = DEFARRSZ/2-1,
};

void convbuf_init(struct conv_buffer * p)
{
	p->next = 0;
	p->flags = 0;
}
void convbuf_clear(struct conv_buffer * p)
{
	struct conv_buffer * n = p->next, *tmp;
	while (n)
	{
		tmp = n;
		n = n->next;
		free(tmp);
	}
	p->next = 0;
	p->flags = 0;
}

char * convbuf_w2a(struct conv_buffer *p, const wchar_t * s, ssize_t len, DWORD cp)
{
	int olen, needlen = -1;
	if (!s) return NULL;
	if (len==-1) len = wcslen(s);
	if (len>>31) return NULL;
	if (!(p->flags & CONVBUF_USED))
	{
		if (len > SP_WSZ)
			needlen = WideCharToMultiByte(cp, 0, s, (int)len, 0, 0, 0, 0);
		if (len <= SP_WSZ || needlen+1 <= DEFARRSZ)
		{
			olen = WideCharToMultiByte(cp, 0, s, (int)len, p->buf, DEFARRSZ, 0, 0);
			if (olen<0) return NULL;
			p->buf[olen] = 0;
			p->flags |= CONVBUF_USED;
			return p->buf;
		}
	}
	if (needlen < 0)
		needlen = WideCharToMultiByte(cp, 0, s, (int)len, 0, 0, 0, 0);
	while (p->next) p = p->next;
	p->next = (struct conv_buffer*)malloc(needlen + 2 + HDR_SIZE);
	convbuf_init(p->next);
	p=p->next;
	olen = WideCharToMultiByte(cp, 0, s, (int)len, p->buf, needlen+1, 0, 0);
	if (olen < 0) return NULL;
	p->buf[olen] = 0;
	p->flags |= CONVBUF_USED;
	return p->buf;
}

#define WBUF ((wchar_t*)p->buf)
wchar_t * convbuf_a2w(struct conv_buffer*p, const char *s, ssize_t len)
{
	int olen, needlen = -1;
	if (!s) return NULL;
	if (len==-1) len = strlen(s);
	if (len>>31) return NULL;
	if (!(p->flags & CONVBUF_USED))
	{
		if (len > SP_ASZ)
			needlen = MultiByteToWideChar(CP_UTF8, 0, s, (int)len, 0, 0);
		if (len <= SP_ASZ || (needlen+1)*sizeof(wchar_t) <= DEFARRSZ)
		{
			olen = MultiByteToWideChar(CP_UTF8, 0, s, (int)len, WBUF, DEFARRSZ/sizeof(wchar_t));
			if (olen<0) return NULL;
			WBUF[olen] = 0;
			p->flags |= CONVBUF_USED;
			return WBUF;
		}
	}
	if (needlen < 0)
		needlen = MultiByteToWideChar(CP_UTF8, 0, s, (int)len, 0, 0);
	while (p->next) p = p->next;
	p->next = (struct conv_buffer*)malloc( (needlen + 2)*sizeof(wchar_t) + HDR_SIZE);
	convbuf_init(p->next);
	p=p->next;

	olen = MultiByteToWideChar(CP_UTF8, 0, s, (int)len, WBUF, needlen+1);
	if (olen < 0) return NULL;
	WBUF[olen] = 0;
	p->flags |= CONVBUF_USED;
	return WBUF;
}

int is_7bits(char * buf, ssize_t len)
{
	ssize_t i;
	if (len < 0)
	{
		for (i=0; buf[i]; ++i)
		{
			if (buf[i] & 0x80)
				return 0;
		}
	}
	else
	{
		for (i=0; i<len; ++i)
			if (buf[i] & 0x80)
				return 0;
	}
	return 1;
}

size_t cb_strlcpy(char * dst, const char* src, size_t dsize)
{
	size_t ncp;
	if (dsize == 0) return 0;
	if (src == 0){*dst = 0; return 0;}

	for (ncp = 0; ncp<dsize; ++ncp)
	{
		dst[ncp] = src[ncp];
		if (src[ncp] == 0) return ncp;
	}
	dst[dsize-1] = 0;
	return dsize - 1;
}
