#include <stdio.h>
#include <errno.h>
#include <termios.h>
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
#include <builtins.h>

shell_t sh_status;

void set_shell_flag_off(unsigned char flag)
{
	sh_status.flags &= ~(1 << flag);
}

void set_shell_flag_on(unsigned char flag)
{
	sh_status.flags |= (1 << flag);
}

bool get_shell_flag(unsigned char flag)
{
	return sh_status.flags & (1 << flag);
}

int execute_command(command_t *c)
{
	pid_t pid;
	int status;

	if((pid = fork()) == -1){
		shell_error(ERR_SHELL_ERROR, "Could not fork %s", *c->array);
#ifdef DEBUG
		debug_error_info();
#endif
		return -1;
	}

	if(pid == 0){
		SET_SIGNALS_PROC;
		if(execvp(*c->array, c->array) == -1){
			shell_error(ERR_NO_SUCH_COM, "%s", *c->array);
#ifdef DEBUG
			debug_error_info();
#endif
			abort();
			return -1;
		}
	}

	setpgid(pid, sh_status.pid);

	while(wait(&status) != pid)
		;

	SET_SIGNALS_SHELL;

	return 0;
}

int initialize_shell(void)
{
	sh_status.terminal = STDIN_FILENO;
	sh_status.is_interactive = isatty(sh_status.terminal);

	if(sh_status.is_interactive){
		sh_status.pgid = getpgrp();
		while(tcgetpgrp(sh_status.terminal) != sh_status.pgid)
			kill(-sh_status.pgid, SIGTTIN);

		SET_SIGNALS_SHELL;

		sh_status.pid = getpid();
		sh_status.pgid = getpid();

		if(sh_status.pgid != getpgrp()){
			if(setpgid(sh_status.pid, sh_status.pgid) < 0){
#ifdef DEBUG
				debug_error_info();
#endif
				shell_error(ERR_SHELL_ERROR, "Failed to put shell in its own process group,"
						" cannot continue\n");
				return -1;
			}
		}

		tcsetpgrp(sh_status.terminal, sh_status.pgid);
		tcgetattr(sh_status.terminal, &sh_status.tmodes);
	}else
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	char *input;
	command_t *c;
	GetLine *gl = new_GetLine(1024, DEFAULT_HISTORY_MEM);

	if(initialize_shell() < 0)
		return -1;

	sh_status.env = initialize_environ();

	TRY_SYS_VAR_IMPORT("home", DEFAULT_HOME);
	TRY_SYS_VAR_IMPORT("path", DEFAULT_PATH);

	add_env_var("prompt", DEFAULT_PROMPT, false, false);

	if(!gl){
		shell_error(ERR_SHELL_ERROR, "Could not initialize libtecla");
		return -1;
	}

	set_shell_flag_on(SHELL_FLAG_REPORT);
	set_shell_flag_on(SHELL_FLAG_RUNNING);
	set_shell_flag_on(SHELL_FLAG_EXPORT);

	while(get_shell_flag(SHELL_FLAG_RUNNING)){
		input = gl_get_line(gl, get_env_var("prompt"), NULL, -1);

		if(!input || *input == '\n')
			continue;

		c = parse(input, gl);

		if(execute_builtins(c->array) == 1)
			continue;
		execute_command(c);
		free_command(c);
	}

	gl = del_GetLine(gl);

	free_command(c);
	free_environ();

	return 0;
}
