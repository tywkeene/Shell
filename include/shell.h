#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>
#include <signal.h>
#include <termios.h>
#include <libtecla.h>

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

/*Shell status flags*/
#define SHELL_FLAG_RUNNING 	1 /*Is the shell running? Obviously 1 unless we get the exit command*/
#define SHELL_FLAG_REPORT 	2 /*Should we report errors?
				    (works on both shell_error() and report_error())*/
#define SHELL_FLAG_EXPORT 	3 /*Should we allow exporting built in variables to the system environment?*/

typedef unsigned char shell_flags_t;

typedef struct command_t{
	char **array;
	unsigned int elements;
}command_t;

typedef struct shell_t{
	shell_flags_t flags;
	pid_t pid;
	pid_t pgid;
	int terminal;
	int is_interactive;
	struct environ_t *env;
	struct termios tmodes;
}shell_t;

void set_shell_flag_off(unsigned char flag);
void set_shell_flag_on(unsigned char flag);
bool get_shell_flag(unsigned char flag);
void free_command(command_t *command);
command_t *parse(char *line, GetLine *gl);
int change_shell_dir(char *path);
int execute_builtins(char **input);
int execute_command(command_t *c);

#endif
