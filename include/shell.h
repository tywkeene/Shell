#ifndef SHELL_H
#define SHELL_H

#define report_error() fprintf(stderr, "[%s: %s():%d] %s\n", __FILE__, \
        	__FUNCTION__, \
        	__LINE__, \
        	strerror(errno));

#define sizeof_array(x) (sizeof(x) / sizeof(*x))

typedef struct command_t{
    	char **array;
    	unsigned int elements;
}command_t;

char *prompt = "$ ";
char *path = "/bin:/usr/bin:/sbin:/usr/local/bin";
char *home;
char *pwd;
pid_t shell_pid;


#endif
