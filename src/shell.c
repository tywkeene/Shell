#include <stdio.h>
#include <errno.h>
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

shell_t sh_status = {true, NULL, 0};

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

void free_command(command_t *command)
{
	int i;

	for(i = 0; i < command->elements; i++)
		free(command->array[i]);
	free(command->array);
	free(command);
	return;
}

command_t *parse(char *line)
{
	unsigned int i;
	char *cur_token;
	char *replaced;
	command_t *ret = malloc(sizeof(command_t));

	ret->elements = count_token(line, " ");
	ret->array = calloc((ret->elements + 1), sizeof(char *));

	cur_token = strtok(line, " ");
	for(i = 0; i < ret->elements; i++){
		if(cur_token[0] == '$'){
			if(replaced = do_replace(cur_token))
				ret->array[i] = copy_string(replaced);
			else
				ret->array[i] = copy_string(cur_token);
			cur_token = strtok(NULL, " ");
			continue;
		}
		ret->array[i] = copy_string(cur_token);
		cur_token = strtok(NULL, " ");
	}
	return ret;
}

int change_shell_dir(char *path)
{
	if(path == NULL)
		return chdir(get_env_var("home"));
	return chdir(path);
}

int execute_builtins(char **input)
{
	int i;
	static const char *builtins[] = {"cd", "exit", "show-env", "edit-env", "add-env", "show-builtins"};

	for(i = 0; i < sizeof_array(builtins); i++)
		if(strncmp(input[0], builtins[i], strlen(builtins[i]) + 1) == 0)
			break;
	switch(i){
	case 0:
		change_shell_dir(input[1]);
		return 1;
	case 1:
		sh_status.running = false;
		return 1;
	case 2:
		show_env();
		return 1;
	case 3:
		if(!input[1] || !input[2]){
			fprintf(stdout, "Usage: %s <var name> <var value>\n", input[0]);
			return 1;
		}
		set_env_var(input[1], input[2]);
		return 1;
	case 4:
		if(!input[1] || !input[2]){
			fprintf(stdout, "Usage: %s <var name> <var value>\n", input[0]);
			return 1;
		}
		add_env_var(input[1], input[2]);
		return 1;
	case 5:
		for(i = 0; i < sizeof_array(builtins); i++)
			fprintf(stdout, "%s ", builtins[i]);
		fprintf(stdout, "\n");
		return 1;
	}
	return 0;
}

int execute_command(command_t *c)
{
	pid_t pid;
	int status;

	if((pid = fork()) == -1){
		shell_error(ERR_SHELL_ERROR, "Could not fork %s", *c->array);
#ifdef DEBUG
		report_error();
#endif
		return -1;
	}

	if(pid == 0){
		SET_SIGNALS_PROC
		if(execvp(*c->array, c->array) == -1){
			shell_error(ERR_NO_SUCH_COM, "%s", *c->array);
#ifdef DEBUG
			report_error();
#endif
			abort();
			return -1;
		}
	}

	setpgid(pid, sh_status.shell_pid);

	while(wait(&status) != pid)
		;

	SET_SIGNALS_SHELL

	return 0;
}

void strip_newline(char *str)
{
	while(*str++)
		if(*str == '\n')
			*str = '\0';
}

int main(int argc, char **argv)
{
	char *input;
	command_t *c;
	GetLine *gl = new_GetLine(1024, 0);

	if(!gl){
		shell_error(ERR_SHELL_ERROR, "Could not initialize libtecla");
		return -1;
	}

	sh_status.env = initialize_environ();

	add_env_var("home", DEFAULT_HOME);
	add_env_var("path", DEFAULT_PATH);
	add_env_var("prompt", DEFAULT_PROMPT);

	sh_status.shell_pid = getpid();
	tcsetpgrp(STDIN_FILENO, sh_status.shell_pid);

	SET_SIGNALS_SHELL

	while(sh_status.running){
		input = gl_get_line(gl, get_env_var("prompt"), NULL, -1);
		strip_newline(input);

		if(!input){
			shell_error(ERR_INVAL_INPUT, "Oops");
			return -1;
		}

		if(strlen(input) < 1)
			continue;

		c = parse(input);
		if(execute_builtins(c->array) == 1)
			continue;
		execute_command(c);
		free_command(c);
	}

	gl = del_GetLine(gl);
	free_environ();

	return 0;
}
