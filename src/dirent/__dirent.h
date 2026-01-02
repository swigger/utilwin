struct __dirstream
{
	HANDLE fd;
	_Bool restart_; //restart scan.
	unsigned short ub_index; //readed index to uni_buffer.
	unsigned short ub_len;
	volatile void* lock;
	char  uni_buffer[4096];
	char  abuf[1024]; //enough for one single dent.
};
