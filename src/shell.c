#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <readline/history.h>
#include <readline/readline.h>

#include <shell.h>

unsigned int count_token(char *string, const char *token_string)
{
    	unsigned int count = 0;
    	char *token;
    	char *copy;
    	copy = malloc(strlen(string) + 1);
    	strncpy(copy, string, strlen(string) + 1);
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
    	command_t *ret = malloc(sizeof(command_t));

    	ret->elements = count_token(line, " ");
    	ret->array = calloc((ret->elements + 1), sizeof(char *));

    	cur_token = strtok(line, " ");
    	for(i = 0; i < ret->elements; i++){
        	ret->array[i] = malloc(strlen(cur_token) + 1);
        	strncpy(ret->array[i], cur_token, strlen(cur_token)  + 1);
        	cur_token = strtok(NULL, " ");
    	}
    	return ret;
}

int change_shell_dir(char *path)
{
    	if(path == NULL)
        	return chdir(home);
    	return chdir(path);
}

void exit_shell(void)
{
    	exit(0);
}

int execute_builtins(char **input)
{
    	int i;
	static const char *builtins[] = {"cd", "exit"};

    	for(i = 0; i < sizeof_array(builtins); i++)
        	if(strncmp(input[0], builtins[i], strlen(builtins[i]) + 1) == 0)
            		break;
    	switch(i){
        	case 0:
            		change_shell_dir(input[1]);
            		return 1;
        	case 1:
            		exit_shell();
            		return 1;
    	}
    	return 0;
}

int execute_command(command_t *c)
{
    	pid_t pid;
    	int status;

    	if((pid = fork()) == -1){
        	report_error();
        	return -1;
    	}

    	if(pid == 0)
        	if(execvp(*c->array, c->array) == -1){
            		report_error();
            		abort();
            		return -1;
        	}

    	setpgid(pid, shell_pid);

    	while(wait(&status) != pid)
        	;
    	return 0;
}

int main(int argc, char **argv)
{
    	char *input;
    	command_t *c;

    	shell_pid = getpid();
    	tcsetpgrp(STDIN_FILENO, shell_pid);

    	home = getenv("HOME");

    	while(1){
        	input = readline(prompt);

		if(!input){
			fprintf(stderr, "Invalid input");
			return -1;
		}

		if(strlen(input) < 1)
			continue;

        	c = parse(input);
		free(input);
		if(execute_builtins(c->array) == 1)
			continue;
        	execute_command(c);
		free_command(c);
    	}
}
