#ifndef HELPERS_H
#define HELPERS_H

/*More verbose error reporting. Only works if DEBUG is defined at compile time*/
#ifdef DEBUG
#define debug_error_info() if(get_shell_flag(SHELL_FLAG_REPORT)) \
	fprintf(stderr, "[%s: %s():%d] %s\n", __FILE__, \
		__FUNCTION__, \
		__LINE__, \
		strerror(errno));
#endif

#define sizeof_array(x) (sizeof(x) / sizeof(*x))

typedef enum cprint_color_t{
	normal,
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	white
}cprint_color_t;

void cprint_msg(FILE *stream, cprint_color_t color, const char *fmt, ...);
void shell_error(int err_type, const char *fmt, ...);
char *copy_string(const char *str);
void strip_newline(char *str);
unsigned int count_token(char *string, const char *token_string);
char *to_lower_varname(char *name);
char *to_upper_varname(char *name);

#endif
