#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
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

int set_sys_env_var(env_var_t *var)
{
	char *upper_name = to_upper_varname(var->name);

	if(setenv(upper_name, var->var, 1) < 0){
#ifdef DEBUG
		report_error()
#endif
			shell_error(ERR_SHELL_ERROR, "Failed to set system environment \
					variable from shell variable: %s\n", var->name);
	}

	free(upper_name);
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
	set_sys_env_var(p);
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
	}else{
		shell_error(ERR_NO_SUCH_VAR, "%s", name);
		return -1;
	}
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
	if(p == NULL)
		shell_error(ERR_NO_SUCH_VAR, "%s", name);
	return p->var;
}

char *do_replace(char *name)
{ 
	char *ret = NULL;
	env_var_t *p;
	env_var_t *next;

	for(p = sh_status.env->vars; p != NULL; p = next){
		if(strstr(name, p->name)){
			ret = p->var;
			break;
		}
		next = p->next;
	}
	return ret;
}

void show_env(void)
{
	env_var_t *p = sh_status.env->vars;
	while(p != NULL){
		fprintf(stdout, "%s:%s\n", p->name, p->var);
		p = p->next;
	}
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
	for(p = sh_status.env->vars; p != NULL; p = next){
		next = p->next;
		free_env_var(p);
	}
}
