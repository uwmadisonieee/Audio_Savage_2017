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

int getFileContent(char* relative_path, char* return_body, int length) {

  FILE* file_p;
  int   content_length;
  char  file_path[512];
  char*  file_ext;
  const char PERIOD = '.';
  
  strcpy(file_path, WEBSITE_FOLDER);
  strcat(file_path, relative_path);  

  // need to decide if file is text or binary
  file_ext = strrchr(relative_path, PERIOD);

  if (NULL == file_ext) {
    return printError("ERROR: getFileContent - no file extension\n", -1);
  }

  // Only opens as text file if one of accepted text file types
  if ( 0 == strcmp(file_ext, ".html") ||
       0 == strcmp(file_ext, ".css")  ||
       0 == strcmp(file_ext, ".js")) {
    file_p = fopen(file_path, "r");
  } else {  
    file_p = fopen(file_path, "rb");
  }
  
  if (file_p == NULL) {
    return printError("ERROR: getFileContent - can't open file\n", -1);
  }
  
  // gets lenght of file and resets
  fseek(file_p, 0, SEEK_END);
  content_length = ftell(file_p);
  fseek(file_p, 0, SEEK_SET);
  
  if (content_length > length) {
    return printError("ERROR: getFileContent - File to large\n", -1);
  }
  
  fread(return_body, content_length, 1, file_p);
  fclose(file_p);
  return content_length;
}
