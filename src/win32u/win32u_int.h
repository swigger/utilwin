#pragma once

struct conv_buffer
{
	struct conv_buffer * next;
	int flags;
	char buf[540];
};

#define CONVBUF_USED 1

#ifdef __cplusplus
extern "C"{
#else
typedef _Bool bool;
#endif


typedef intptr_t ssize_t;
void convbuf_init(struct conv_buffer * p);
void convbuf_clear(struct conv_buffer * p);
char * convbuf_w2a(struct conv_buffer *p, const wchar_t * s, ssize_t len, DWORD cp);
wchar_t * convbuf_a2w(struct conv_buffer*p, const char *s, ssize_t len);
int is_7bits(char * buf, ssize_t len);

size_t cb_strlcpy(char * dst, const char* src, size_t dsize);
#define strlcpy cb_strlcpy

#ifdef __cplusplus
}
#endif
