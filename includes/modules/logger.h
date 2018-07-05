#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string.h>

enum LOGGER_STDOUT_OPTION {LOG_FILE, LOG_STDOUT};

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

void log_print(char *filename, int line, int stdout_print, char *fmt, ...);

#define LOG_PRINT(...) log_print(__FILENAME__, __LINE__, __VA_ARGS__)

#define LOG_ERRO(...) log_print(__FILENAME__, __LINE__, LOG_STDOUT, __VA_ARGS__)

#endif /* __LOGGER_H__ */
