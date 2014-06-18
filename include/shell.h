#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>

#define report_error() fprintf(stderr, "[%s: %s():%d] %s\n", __FILE__, \
		__FUNCTION__, \
		__LINE__, \
		strerror(errno));

#define sizeof_array(x) (sizeof(x) / sizeof(*x))

#define ERR_NO_SUCH_VAR 0
#define ERR_NO_SUCH_COM 1
#define ERR_NO_SUCH_FIL 2
#define ERR_INVAL_INPUT 3
#define ERR_SHELL_ERROR 4

typedef struct command_t{
	char **array;
	unsigned int elements;
}command_t;

typedef struct shell_t{
	bool running;
	struct environ_t *env;
	pid_t shell_pid;
	char *pwd;
}shell_t;

void shell_error(int err_type, const char *fmt, ...);
char *copy_string(const char *str);
unsigned int count_token(char *string, const char *token_string);
void free_command(command_t *command);
command_t *parse(char *line);
int change_shell_dir(char *path);
int execute_builtins(char **input);
int execute_command(command_t *c);

#endif
