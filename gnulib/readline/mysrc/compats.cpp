#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#define READLINE_LIBRARY
#include "readline.h"
#include "termio.h"
#include <process.h>
#include <io.h>
#include <signal.h>
#include <assert.h>
#include <locale.h>
#include <string>
#include <cuchar>
#include <queue>
#include "_win.h"

using std::string;

#define extc extern "C"

static int64_t next_stop = 0;
extc int wrl_alarm(int seconds) {
	if (seconds<=0) next_stop = 0;
	else next_stop = GetTickCount64() + seconds * 1000;
	return 0;
}
/// 182521 chars in total
struct urange_t {
	uint32_t start, end;
};
static const urange_t w_chars[] = {
	{0x1100, 0x115F},
	{0x231A, 0x231B},
	{0x2329, 0x232A},
	{0x23E9, 0x23EC},
	{0x23F0, 0x23F0},
	{0x23F3, 0x23F3},
	{0x25FD, 0x25FE},
	{0x2614, 0x2615},
	{0x2648, 0x2653},
	{0x267F, 0x267F},
	{0x2693, 0x2693},
	{0x26A1, 0x26A1},
	{0x26AA, 0x26AB},
	{0x26BD, 0x26BE},
	{0x26C4, 0x26C5},
	{0x26CE, 0x26CE},
	{0x26D4, 0x26D4},
	{0x26EA, 0x26EA},
	{0x26F2, 0x26F3},
	{0x26F5, 0x26F5},
	{0x26FA, 0x26FA},
	{0x26FD, 0x26FD},
	{0x2705, 0x2705},
	{0x270A, 0x270B},
	{0x2728, 0x2728},
	{0x274C, 0x274C},
	{0x274E, 0x274E},
	{0x2753, 0x2755},
	{0x2757, 0x2757},
	{0x2795, 0x2797},
	{0x27B0, 0x27B0},
	{0x27BF, 0x27BF},
	{0x2B1B, 0x2B1C},
	{0x2B50, 0x2B50},
	{0x2B55, 0x2B55},
	{0x2E80, 0x2E99},
	{0x2E9B, 0x2EF3},
	{0x2F00, 0x2FD5},
	{0x2FF0, 0x303E},
	{0x3041, 0x3096},
	{0x3099, 0x30FF},
	{0x3105, 0x312F},
	{0x3131, 0x318E},
	{0x3190, 0x31E3},
	{0x31EF, 0x321E},
	{0x3220, 0x3247},
	{0x3250, 0x4DBF},
	{0x4E00, 0xA48C},
	{0xA490, 0xA4C6},
	{0xA960, 0xA97C},
	{0xAC00, 0xD7A3},
	{0xF900, 0xFAFF},
	{0xFE10, 0xFE19},
	{0xFE30, 0xFE52},
	{0xFE54, 0xFE66},
	{0xFE68, 0xFE6B},
	{0xFF01, 0xFF60},
	{0xFFE0, 0xFFE6},
	{0x16FE0, 0x16FE4},
	{0x16FF0, 0x16FF1},
	{0x17000, 0x187F7},
	{0x18800, 0x18CD5},
	{0x18D00, 0x18D08},
	{0x1AFF0, 0x1AFF3},
	{0x1AFF5, 0x1AFFB},
	{0x1AFFD, 0x1AFFE},
	{0x1B000, 0x1B122},
	{0x1B132, 0x1B132},
	{0x1B150, 0x1B152},
	{0x1B155, 0x1B155},
	{0x1B164, 0x1B167},
	{0x1B170, 0x1B2FB},
	{0x1F004, 0x1F004},
	{0x1F0CF, 0x1F0CF},
	{0x1F18E, 0x1F18E},
	{0x1F191, 0x1F19A},
	{0x1F200, 0x1F202},
	{0x1F210, 0x1F23B},
	{0x1F240, 0x1F248},
	{0x1F250, 0x1F251},
	{0x1F260, 0x1F265},
	{0x1F300, 0x1F320},
	{0x1F32D, 0x1F335},
	{0x1F337, 0x1F37C},
	{0x1F37E, 0x1F393},
	{0x1F3A0, 0x1F3CA},
	{0x1F3CF, 0x1F3D3},
	{0x1F3E0, 0x1F3F0},
	{0x1F3F4, 0x1F3F4},
	{0x1F3F8, 0x1F43E},
	{0x1F440, 0x1F440},
	{0x1F442, 0x1F4FC},
	{0x1F4FF, 0x1F53D},
	{0x1F54B, 0x1F54E},
	{0x1F550, 0x1F567},
	{0x1F57A, 0x1F57A},
	{0x1F595, 0x1F596},
	{0x1F5A4, 0x1F5A4},
	{0x1F5FB, 0x1F64F},
	{0x1F680, 0x1F6C5},
	{0x1F6CC, 0x1F6CC},
	{0x1F6D0, 0x1F6D2},
	{0x1F6D5, 0x1F6D7},
	{0x1F6DC, 0x1F6DF},
	{0x1F6EB, 0x1F6EC},
	{0x1F6F4, 0x1F6FC},
	{0x1F7E0, 0x1F7EB},
	{0x1F7F0, 0x1F7F0},
	{0x1F90C, 0x1F93A},
	{0x1F93C, 0x1F945},
	{0x1F947, 0x1F9FF},
	{0x1FA70, 0x1FA7C},
	{0x1FA80, 0x1FA88},
	{0x1FA90, 0x1FABD},
	{0x1FABF, 0x1FAC5},
	{0x1FACE, 0x1FADB},
	{0x1FAE0, 0x1FAE8},
	{0x1FAF0, 0x1FAF8},
	{0x20000, 0x2FFFD},
	{0x30000, 0x3FFFD}
};
constexpr uint32_t w_chars_len = 120;
extc int wrl_wcwidth(uint32_t wch)
{
	if (wch < w_chars[0].start && wch >= 0x20) // [[likely]]
		return 1;
	if (wch < 0x20) return 0;
	auto p = std::lower_bound(w_chars, w_chars + w_chars_len, wch, [](const urange_t& a, uint32_t b) {
		return a.start < b;
	});
	if (p > w_chars && p[-1].start <= wch && wch <= p[-1].end) return 2;
	if (p != w_chars + w_chars_len && p->start == wch) return 2;
	return 1;
}

extc size_t wrl_mbrtoc32(char32_t* pc32, const char* s, size_t n, mbstate_t* ps)
{
	return std::mbrtoc32(pc32, s, n, ps);
}

extc size_t wrl_c32rtomb(char* s, char32_t c32, mbstate_t* ps)
{
	return std::c32rtomb(s, c32, ps);
}

extc int wrl_kill(int pid, int sig) {
	if (sig == SIGINT) {
		GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
		_exit(3);
		return 0;
	}
	errno = ENOENT;
	return -1;
}

extc int wrl_readlink(const char *path, char *buf, size_t bufsiz) {
	errno = ENOENT;
	return -1;
}

extc int wrl_setenv(const char *name, const char *value, int overwrite) {
	size_t destlen = strlen(name) + strlen(value) + 8;
	char * buf = (char *)malloc(destlen);
	if (buf == NULL) {
		errno = ENOMEM;
		return -1;
	}
	snprintf(buf, destlen, "%s=%s", name, value);
	int ret = _putenv(buf);
	free(buf);
	return ret;
}
extc char* wrl_getenv(const char* name)
{
	if (strcmp(name, "LC_ALL") == 0) return (char*)"en_US.UTF-8";
	if (strcmp(name, "LANG") == 0) return (char*)"en_US.UTF-8";
	return getenv(name);
}

extc int wrl_gettimeofday(struct timeval *tv, struct timezone *tz) {
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	ULARGE_INTEGER uli;
	uli.LowPart = ft.dwLowDateTime;
	uli.HighPart = ft.dwHighDateTime;
	uli.QuadPart -= 116444736000000000;
	uli.QuadPart /= 10;
	tv->tv_sec = (long)(uli.QuadPart / 1000000);
	tv->tv_usec = (long)(uli.QuadPart % 1000000);
	return 0;
}

static int get_stdin_nb(void);
static void init_win_console_io()
{
	static bool inited = false;
	if (inited) return;
	inited = true;
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, "en_US.UTF-8");
}

extc int wrl_ioctl(int fd, unsigned long request, intptr_t val0) {
	HANDLE hfd = (HANDLE)_get_osfhandle(fd);
	DWORD oldMode = 0;
	if (GetConsoleMode(hfd, &oldMode))
	{
		init_win_console_io();
		if (request == TCGETA)
		{
			auto& tio = *(struct termio*)val0;
			memset(&tio, 0, sizeof(tio));
			tio.c_iflag = ICRNL | IXON;
			tio.c_oflag = OPOST | ONLCR;
			tio.c_cflag = CREAD | CS8 | B38400; //c_cflag 0xbf
			tio.c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN; //0o105073
			tio.c_line = 0;
			tio.c_cc[VINTR] = 0x03;
			tio.c_cc[VQUIT] = 0x1c;
			tio.c_cc[VERASE] = 0x7f;
			tio.c_cc[VKILL] = 0x15;
			tio.c_cc[VEOF] = 0x04;
			tio.c_cc[VEOL] = 0x00;
			tio.c_cc[VMIN] = 0x01;
			//tio.c_cc[VSWTC] = 0x00;
			return 0;
		}
		else if (request == TCSETA || request == TCSETAW)
		{
			auto& tio = *(struct termio*)val0;
			// removed ICRNL, map cr to nl.
			oldMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
			if (tio.c_lflag & (ICANON | ECHO)) {
				SetConsoleMode(hfd, oldMode | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
			} else {
				SetConsoleMode(hfd, oldMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT));
			}
			return 0;
		}
		else if (request == FIONREAD) {
			*(int*)val0 = get_stdin_nb();
			return 0;
		}
		else if (request == TIOCGWINSZ) {
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
			auto ws = (struct winsize*)val0;
			ws->ws_col = csbi.dwSize.X; // csbi.srWindow.Right - csbi.srWindow.Left + 1;
			ws->ws_row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
			return 0;
		}
		else if (request == TIOCSWINSZ) {
			// ignore
			return 0;
		}
	}
	Sleep(0);
	errno = ENOENT;
	return -1;
}

extc int tputs(const char* str, int affcnt, int (*putc)(int))
{
	if (!str) return 0;
	while (*str)
	{
		if (*str == '\n') putc('\r');
		putc(*str);
		str++;
	}
	return 0;
}

extc int tgetent(char* bp, const char* name) {
	// ignores bp.
	return 1;
}

extc int tgetflag(const char* id) {
	if (strcmp(id, "xn") == 0) { // what is xn? number of col in cell? xon? I'm not sure.
		return 1;
	}
	if (strcmp(id, "am") == 0) { // auto margin/auto wrap.
		return 1;
	}
	if (strcmp(id, "km") == 0) { // keypad mode? meta key?
		return 0;
	}
	Sleep(0);
	return 0;
}

extc int tgetnum(const char* id) {
	Sleep(0);
	return 0;
}

extc char* tgetstr(const char* id, char** area) {
	const char* mtbl[][2] = {
		{"@7", "\033OF"},  // Save cursor position and attributes
		{"DC", "\033[%p1%dP"}, // Delete %p1%d characters.
		{"E3", "\033[3J"},  // Erase Char
		{"IC", "\033[%p1%d@"}, // Insert Char
		{"ce", "\033[K"}, // Clear to end of line
		{"cl", "\033[H\033[2J"}, // clear screen
		{"cr", "\r"},  // carriage return
		{"dc", "\033[P"},  // delete char method2.
		{"ei", "\033[4l"},  // End insert mode.
		{"im", "\033[4h"},  // Insert mode.
		{"kD", "\033[3~"},  // Delete key
		{"kH", "\033[1~"},  // Home key
		{"kI", "\033[2~"},  // Insert key
		{"kN", "\033[6~"},  // Next key.
		{"kP", "\033[5~"},  // Previous key.
		{"kd", "\033[B"},  // Down key, dhlr
		{"ke", "\033[?1l\033>"}, // Keypad mode
		{"kh", "\033OH"}, // Home key
		{"kl", "\033OD"}, // Left key
		{"kr", "\033OC"}, // Right key
		{"ks", "\033[?1h\033="}, // Keypad mode start
		{"ku", "\033[A"},  // up key
		{"le", "\b"},  // left cursor key.
		{"mm", "\033[?1034h"}, // meta mode.
		{"mo", "\033[?1034l"}, // meta mode On? OFF?
		{"nd", "\033[C"},  // right cursor key.
		{"se", "\033[27m"}, // end standout mode.
		{"so", "\033[7m"},  // standout mode.
		{"up", "\033[A"}, // up cursor key.
		{"vb", "\033[?5h\033[?5l"}, // visual bell.
		{"vs", "\033[?12;25h"}, // cursor visible.
		{"ve", "\033[?12l\033[?25h"}, // cursor invisible.
		{0, 0}
	};
	for (int i=0; mtbl[i][0]; i++) {
		if (strcmp(mtbl[i][0], id) == 0) {
			return (char*)mtbl[i][1];
		}
	}
	return 0;
}

extc char* tgoto(const char* cap, int col, int row) {
	static std::string sbuf;
	sbuf.clear();
	enum state_t {
		ST_NORMAL, ST_PERCENT, ST_P, 
	} st = ST_NORMAL;
	int pval = 0;
	for (int i = 0; cap[i]; ++i)
	{
		if (st == ST_NORMAL && cap[i] == '%') st = ST_PERCENT;
		else if (st == ST_PERCENT && cap[i] == 'p') st = ST_P;
		else if (st == ST_PERCENT && cap[i] == 'd') { sbuf += std::to_string(pval); st = ST_NORMAL; }
		else if (st == ST_P && isdigit(cap[i])) {
			if (cap[i] == '1') pval = row;
			else if (cap[i] == '2') pval = col;
			else pval = 0;
			st = ST_NORMAL;
		} else {
			st = ST_NORMAL;
			sbuf += cap[i];
		}
	}
	return (char*)sbuf.c_str();
}


struct CConsoleReader
{
public:
	HANDLE m_Event;
	std::deque<char> m_pub_buf;

protected:
	FILE* m_file;
	string m_buf;
	uint16_t m_chtmp;
	bool broken_ = false;
	bool winsz_changed_ = false;
	int ctrl_down = 0;
	int shift_down = 0;
	CRITICAL_SECTION m_lock;

public:
	static CConsoleReader* anew(FILE* f) {
		CConsoleReader* r = new CConsoleReader(f);
		_beginthreadex(NULL, 0, CConsoleReader::run_thread, r, 0, NULL);
		Sleep(0); // yield to the new thread
		return r;
	}
	size_t move_data()
	{
		EnterCriticalSection(&m_lock);
		m_pub_buf.insert(m_pub_buf.end(), m_buf.begin(), m_buf.end());
		m_buf.clear();
		auto rr = m_pub_buf.size();
		LeaveCriticalSection(&m_lock);
		return rr;
	}
	bool broken() const {
		return broken_;
	}
	bool winsz_changed() {
		bool r = winsz_changed_;
		winsz_changed_ = false;
		return r;
	}
protected:
	void on(WINDOW_BUFFER_SIZE_RECORD& r)
	{
		winsz_changed_ = true;
	}

	void add_wch(string& buf, uint16_t wch)
	{
		if (!wch) return;
		if (wch <= 0x7f)
			buf += (char)wch;
		// convert to utf-8.
		else if (wch <= 0x7ff) {
			buf += (char)(0xc0 | (wch >> 6));
			buf += (char)(0x80 | (wch & 0x3f));
			m_chtmp = 0;
		}
		else if (wch >= 0xd800 && wch < 0xdc00) {
			m_chtmp = wch;
		}
		else if (wch >= 0xdc00 && wch <= 0xdfff) {
			int wch2 = 0x10000 + ((m_chtmp - 0xd800) << 10) + (wch - 0xdc00);
			buf += (char)(0xf0 | (wch2 >> 18));
			buf += (char)(0x80 | ((wch2 >> 12) & 0x3f));
			buf += (char)(0x80 | ((wch2 >> 6) & 0x3f));
			buf += (char)(0x80 | (wch2 & 0x3f));
			m_chtmp = 0;
		}
		else {
			buf += (char)(0xe0 | (wch >> 12));
			buf += (char)(0x80 | ((wch >> 6) & 0x3f));
			buf += (char)(0x80 | (wch & 0x3f));
			m_chtmp = 0;
		}
	}
	void on(string& buf, const KEY_EVENT_RECORD& ker)
	{
		if (!ker.bKeyDown) {
			switch (ker.wVirtualKeyCode) {
			case VK_CONTROL: ctrl_down = 0; break;
			case VK_SHIFT: shift_down = 0; break;
			default: break;
			}
			return;
		}
		switch (ker.wVirtualKeyCode)
		{
		case VK_CONTROL: ctrl_down = 1; break;
		case VK_SHIFT: shift_down = 1; break;
		default: add_wch(buf, ker.uChar.UnicodeChar); break;
		}
	}
	static uint32_t run_thread(void* this_0) {
		CConsoleReader* this_ = (CConsoleReader*)this_0;
		HANDLE osh = (HANDLE)_get_osfhandle(_fileno(this_->m_file));
		DWORD mode = 0;
		GetConsoleMode(osh, &mode);

		string local_buf;
		while (1) {
			INPUT_RECORD ir[100];
			DWORD n;
			local_buf.clear();
			ReadConsoleInputW(osh, &ir[0], _countof(ir), &n);
			for (DWORD i = 0; i < n; ++i) {
				switch (ir[i].EventType)
				{
				case KEY_EVENT:
					this_->on(local_buf, ir[i].Event.KeyEvent);
					break;
				case WINDOW_BUFFER_SIZE_EVENT: // ignore window resize events
					this_->on(ir[i].Event.WindowBufferSizeEvent);
					break;
				case MOUSE_EVENT: // ignore mouse events
				case MENU_EVENT: // ignore menu events
				case FOCUS_EVENT: // ignore focus events
					break;
				default: assert(false); break;
				}
			}
			if (local_buf.size()) {
				EnterCriticalSection(&this_->m_lock);
				this_->m_buf += local_buf;
				LeaveCriticalSection(&this_->m_lock);
				SetEvent(this_->m_Event);
			}
		}
	}

	CConsoleReader(FILE* f) : m_file(f)
	{
		InitializeCriticalSection(&m_lock);
		m_Event = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
};

static CConsoleReader* g_reader = 0;

extc int rl_getc(FILE* stream)
{
	assert(stream == stdin);
	if (!g_reader)
		g_reader = CConsoleReader::anew(stream);
	auto reader = g_reader;

	for (; !reader->broken(); )
	{
		// no need lock for pub buf.
		if (!reader->m_pub_buf.empty() || reader->move_data()) {
			int c = reader->m_pub_buf.front() & 0xff;
			reader->m_pub_buf.pop_front();
			if (c == 3) _rl_signal_handler(SIGINT);
			if (reader->winsz_changed()) _rl_signal_handler(SIGWINCH);
			return c;
		}
		// determin the timeout to wait.
		DWORD timeout = INFINITE;
		if (next_stop) {
			int64_t to1 = next_stop - GetTickCount64();
			if (to1 <= 0) timeout = 0;
			else if (to1 > INT_MAX) timeout = INFINITE;
			else timeout = (DWORD)to1;
		}
		int q = WaitForSingleObject(reader->m_Event, timeout);
		if (q == WAIT_TIMEOUT) {
			next_stop = 0;
			_rl_signal_handler(SIGALRM);
		}
	}
	return EOF;
}

static int get_stdin_nb() {
	if (!g_reader) return 0;
	return (int) g_reader->move_data();
}
