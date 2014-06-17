#ifndef ENV_H
#define ENV_H

#define DEFAULT_PROMPT 	"$ "
#define DEFAULT_PATH 	"/bin:/usr/bin/:/sbin/:/usr/local/bin"
#define DEFAULT_HOME 	"/"

typedef struct env_var_t{
	char *name;
	char *var;
	struct env_var_t *next;
}env_var_t;

typedef struct environ_t{
	env_var_t *vars;
}environ_t;

env_var_t *alloc_env_var(char *name, char *new);
environ_t *initialize_environ(void);
void add_env_var(char *name, char *var);
int set_env_var(char *name, char *set);
char *get_env_var(char *name);
char *do_replace(char *name);
void show_env(void);
void free_env_var(env_var_t *var);
void free_environ(void);

#endif
