#ifndef BUILTINS_H
#define BUILTINS_H

typedef struct command_t{
	char **array;
	unsigned int elements;
}command_t;

void free_command(command_t *command);
command_t *parse(char *line, GetLine *gl);
int change_shell_dir(char *path);
int execute_builtins(char **input);
#endif
