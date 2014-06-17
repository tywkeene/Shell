#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <shell.h>
#include <env.h>

extern shell_t sh_status;

env_var_t *alloc_env_var(char *name, char *new)
{
	env_var_t *var = calloc(1, sizeof(env_var_t));
	var->name = copy_string(name);
	var->var = copy_string(new);
	var->next = NULL;
	return var;
}

environ_t *initialize_environ(void)
{
	environ_t *env = calloc(1, sizeof(environ_t));
	env->vars = NULL;
	return env;
}

void add_env_var(char *name, char *var)
{
	env_var_t *p = sh_status.env->vars;
	env_var_t *new = alloc_env_var(name, var);

	if(p == NULL){
		p = new;
		p->next = NULL;
		sh_status.env->vars = p;
	}else{
		sh_status.env->vars = p;
		while(p->next != NULL)
			p = p->next;
		p->next = new;
		p = new;
		p->next = NULL;
	}
}

int set_env_var(char *name, char *set)
{
	env_var_t *p = sh_status.env->vars;
	while(p != NULL){
		if(strncmp(p->name, name, strlen(p->name) + 1) == 0)
			break;
		p = p->next;
	}
	if(p){
		free(p->var);
		p->var = copy_string(set);
	}else
		return -1;
	return 0;
}

char *get_env_var(char *name)
{
	env_var_t *p = sh_status.env->vars;
	while(p != NULL){
		if(strncmp(p->name, name, strlen(p->name) + 1) == 0)
			break;
		p = p->next;
	}
	return p->var;
}

void free_env_var(env_var_t *var)
{
	free(var->name);
	free(var->var);
	free(var);
}

void free_environ(void)
{
	env_var_t *p;
	env_var_t *next;
	for (p = sh_status.env->vars; p != NULL; p = next) {
		next = p->next;
		free_env_var(p);
	}
}
