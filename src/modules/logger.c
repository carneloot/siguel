/**
 * Codigo original por Varun Gupta
 * Link para o site:
 * http://simplestcodings.blogspot.com/2010/10/simple-logger-in-c.html
 *
 * Alterado por Matheus Carnelutt
 */

#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE *fp;

#ifdef DEBUG
static int SESSION_TRACKER;  // Keeps track of session

/*
  Gets the current date/time and returns it as a string of the form:
  [yyyy-mm-dd hh:mm:ss]
  Returned char pointer must be freed.
*/
static char *getDateString() {
  // Initialize and get current time
  time_t t = time(NULL);

  // Allocate space for date string
  char *date = (char *) malloc(100);

  // Format the time correctly
  strftime(date, 100, "[%F %T]", localtime(&t));

  return date;
}
#endif

void log_print(char *filename, int line, int stdout_print, char *fmt, ...) {
  va_list list;

  if (stdout_print) {
    va_start(list, fmt);
    vfprintf(stdout, fmt, list);
    va_end(list);
    fputc('\n', stdout);
  }

  #ifdef DEBUG

  char *time_string;

  if (SESSION_TRACKER > 0)
    fp = fopen("log.txt", "a");
  else
    fp = fopen("log.txt", "w");

  time_string = getDateString();
  fprintf(fp, "%s ", time_string);
  free(time_string);

  fprintf(fp, "%s:%d: ", filename, line);

  va_start(list, fmt);
  vfprintf(fp, fmt, list);
  va_end(list);

  fputc('\n', fp);

  SESSION_TRACKER++;

  fclose(fp);

  #endif
}
