#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <libtecla.h>

#include <shell.h>
#include <env.h>
#include <helpers.h>

extern shell_t sh_status;

void shell_error(int err_type, const char *fmt, ...)
{
	static const char *err_str[] = {"No such environment variable", "No such command", "No such file",
		"Invalid input", "Shell error"};
	va_list args;
	va_start(args, fmt);
	fflush(stdout);
	fflush(stderr);
	fprintf(stderr, "Shell error %d: ", err_type);
	fprintf(stderr, "%s: ", err_str[err_type]);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	fflush(stderr);
	va_end(args);
}

char *copy_string(const char *str)
{
	char *new = calloc(1, strlen(str) + 1);
	strncpy(new, str, strlen(str) + 1);
	return new;
}

void strip_newline(char *str)
{
	while(*str++)
		if(*str == '\n')
			*str = '\0';
}

unsigned int count_token(char *string, const char *token_string)
{
	unsigned int count = 0;
	char *token;
	char *copy;
	copy = copy_string(string);
	for(token = strtok(copy, token_string); token != NULL;
			token = strtok(NULL, token_string))
		count++;
	free(copy);
	return count;
}

char *to_lower_varname(char *name)
{
	int i;
	char *out = calloc(1, strlen(name) + 1);
	ssize_t len = strlen(name);

	for(i = 0; i < len; i++)
		out[i] = (char) tolower((int) name[i]);
	return out;
}

char *to_upper_varname(char *name)
{
	int i;
	char *out = calloc(1, strlen(name) + 1);
	ssize_t len = strlen(name);

	for(i = 0; i < len; i++)
		out[i] = (char) toupper((int) name[i]);
	return out;
}
