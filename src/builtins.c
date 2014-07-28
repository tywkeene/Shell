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

#include <shell.h>
#include <env.h>
#include <helpers.h>
#include <builtins.h>

void free_command(command_t *command)
{
	int i;
	if(!command)
		return;

	for(i = 0; i < command->elements; i++)
		free(command->array[i]);
	free(command->array);
	free(command);
	return;
}

command_t *parse(char *line, GetLine *gl)
{
	unsigned int i;
	char *cur_token;
	char *replaced;
	command_t *ret = malloc(sizeof(command_t));

	strip_newline(line);
	gl_append_history(gl, line);

	ret->elements = count_token(line, " ");
	ret->array = calloc((ret->elements + 1), sizeof(char *));

	cur_token = strtok(line, " ");
	for(i = 0; i < ret->elements; i++){
		if(cur_token[0] == '$'){
			if(replaced = do_var_name_substitution(cur_token))
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
	static const char *builtins[] = {"cd", "exit", "show-vars", "edit-var", "add-var", "show-builtins",
		"export-var"};

	for(i = 0; i < sizeof_array(builtins); i++)
		if(strncmp(input[0], builtins[i], strlen(builtins[i]) + 1) == 0)
			break;
	switch(i){
	case 0: /*cd*/
		if(change_shell_dir(input[1]) != 0){
#ifdef DEBUG
			debug_error_info();
#endif
			shell_error(ERR_SHELL_ERROR, "Failed to cd to '%s'", input[1]);
		}
		return 1;
	case 1: /*exit*/
		set_shell_flag_off(SHELL_FLAG_RUNNING);
		return 1;
	case 2: /*show-vars*/
		show_env();
		return 1;
	case 3: /*edit-var*/
		if(!input[1] || !input[2]){
			fprintf(stdout, "Usage: %s <target var name> <new var value>\n", input[0]);
			return 1;
		}
		set_env_var(input[1], input[2]);
		return 1;
	case 4: /*add-var*/
		if(!input[1] || !input[2]){
			fprintf(stdout, "Usage: %s <new var name> <new var value>\n", input[0]);
			return 1;
		}
		add_env_var(input[1], input[2], true, false);
		return 1;
	case 5: /*show-builtins*/
		for(i = 0; i < sizeof_array(builtins); i++)
			fprintf(stdout, " %s ", builtins[i]);
		fprintf(stdout, "\n");
		return 1;
	case 6: /*export-var*/
		if(get_shell_flag(SHELL_FLAG_EXPORT)){
			if(!input[1]){
				fprintf(stdout, "Usage: %s <name of built-in var to export>\n", input[0]);
				return 1;
			}
			export_sys_env_var(input[1]);
		}
		return 1;
	}
	return 0;
}
