#pragma once
#define SIGWINCH      28
#define SIGALRM       14

#ifdef __cplusplus
extern "C" {
#endif
	extern int volatile _rl_caught_signal;
	void _rl_signal_handler(int sig);
#ifdef __cplusplus
}
#endif // __cplusplus
