/*! Printing on stdout, reading from stdin */

#include <api/stdio.h>

#include <lib/string.h>
#include <types/io.h>
#include <api/errno.h>

int _errno;	/* Error number that represent last syscall error */

console_t *u_stdout, *u_stderr;

/*! Initialize standard descriptors (input, output, error) */
int stdio_init()
{
	extern console_t U_STDOUT, U_STDERR;

	u_stdout = &U_STDOUT;
	u_stdout->init(0);

	u_stderr = &U_STDERR;
	u_stderr->init(0);

	return EXIT_SUCCESS;
}

/*! Formated output to console (lightweight version of 'printf') */
int printf(char *format, ...)
{
	char text[CONSOLE_MAXLEN];

	vssprintf(text, CONSOLE_MAXLEN, &format);

	return u_stdout->print(text);
}

/*! Formated output to error console */
void warn(char *format, ...)
{
	char text[CONSOLE_MAXLEN];

	vssprintf(text, CONSOLE_MAXLEN, &format);

	u_stderr->print(text);
}

int PRINTF(char* text) {
	char textbuf[CONSOLE_MAXLEN];

	vssprintf(textbuf, CONSOLE_MAXLEN, &text);

	for (int i = 0; i < strlen(textbuf); i++) {
		if (textbuf[i] >= 'a' && textbuf[i] <= 'z') {
			textbuf[i] -= ('a' - 'A');
		}
	}

	return u_stdout->print(textbuf);
}