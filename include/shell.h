#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>
#include <signal.h>
#include <libtecla.h>

/*More verbose error reporting. Only works if DEBUG is defined at compile time*/
#ifdef DEBUG
#define report_error() fprintf(stderr, "[%s: %s():%d] %s\n", __FILE__, \
		__FUNCTION__, \
		__LINE__, \
		strerror(errno));
#endif

#define sizeof_array(x) (sizeof(x) / sizeof(*x))

/*Ignore most signals since we're an interactive shell*/
#define SET_SIGNALS_SHELL \
	signal(SIGINT, SIG_IGN); \
signal(SIGQUIT, SIG_IGN); \
signal(SIGTSTP, SIG_IGN); \
signal(SIGTTIN, SIG_IGN); \
signal(SIGTTOU, SIG_IGN); \
signal(SIGTERM, SIG_IGN); \
signal(SIGCHLD, SIG_DFL);

/*Set most signals back to default for spawned processes*/
#define SET_SIGNALS_PROC \
	signal(SIGINT, SIG_DFL); \
signal(SIGQUIT, SIG_DFL); \
signal(SIGTSTP, SIG_DFL); \
signal(SIGTTIN, SIG_DFL); \
signal(SIGTTOU, SIG_DFL); \
signal(SIGTERM, SIG_DFL);

/*Error codes for shell_error() in shell.c*/
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
}shell_t;

void free_command(command_t *command);
command_t *parse(char *line, GetLine *gl);
int change_shell_dir(char *path);
int execute_builtins(char **input);
int execute_command(command_t *c);

#endif
