#ifndef ENV_H
#define ENV_H

/*Default environment variables. Set right after the environment is initialized*/
#define DEFAULT_PROMPT 	"$ "
#define DEFAULT_PATH 	"/bin:/usr/bin/:/sbin/:/usr/local/bin"
#define DEFAULT_HOME 	"/"

#define DEFAULT_HISTORY_MEM 100

/*Try to import a system environment variable, use def if we can't import from
 * system*/
#define TRY_SYS_VAR_IMPORT(varname, def) if(import_sys_env_var(varname) < 0){ \
	shell_error(ERR_SHELL_ERROR, "Could not import %s from system, using default: %s\n", varname, def); \
	add_env_var(varname, def, false, true); \
			}

typedef struct env_var_t{
	bool imported;
	bool user_set;
	char *name;
	char *var;
	struct env_var_t *next;
}env_var_t;

typedef struct environ_t{
	env_var_t *vars;
}environ_t;

env_var_t *alloc_env_var(char *name, char *new);
environ_t *initialize_environ(void);
void export_sys_env_var(char *name);
int import_sys_env_var(char *name);
env_var_t *find_env_var(char *name);
void add_env_var(char *name, char *var, bool is_user_set, bool is_import);
int set_env_var(char *name, char *set);
char *get_env_var(char *name);
char *do_var_name_substitution(char *name);
void show_env(void);
void free_env_var(env_var_t *var);
void free_environ(void);

#endif
