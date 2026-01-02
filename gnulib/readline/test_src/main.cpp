#pragma warning(disable: 4005)  // macro redefinition
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <Windows.h>

int main()
{
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	// rl_getc_function = my_getc;

	for (;;)
	{
		rl_set_timeout(50, 0);
		char * s = readline("Enter a string: ");
		if (s == NULL)
			break;
		if (*s == 0) continue;
		printf("You entered: %s\n", s);
		add_history(s);
	}
	return 0;
}
