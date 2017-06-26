#include "util.h"

int printError(char* message, int return_val) {
  if (1 == PRINT_ERRORS) {
    printf("%s\n", message);
  }
  return return_val;
}

void getTime(char** timestamp, int length) {

  time_t raw_time;
  struct tm *info;
  time( &raw_time );

  info = localtime( &raw_time );

  strftime(*timestamp, length, "%a, %d %b %Y %H:%M:%S %Z", info);  
}
