#include "util.h"

void getTime(char** timestamp, int length) {

  time_t raw_time;
  struct tm *info;
  time( &raw_time );

  info = localtime( &raw_time );

  strftime(*timestamp, length, "%a, %d %b %Y %H:%M:%S %Z", info);  
}

int getHTML(char* file, char** html, int length) {

  FILE* file_p;
  int content_length;
  char file_path[512];

  strcpy(file_path, WEBSITE_FOLDER);
  strcat(file_path, file);  
  
  file_p = fopen(file_path, "rb");
  printf("file_path %s\n",file_path);
  
  if (file_p == NULL) {
    printf("ERROR: getHTML - can't open file\n");
    return -1;
  }
  
  // gets lenght of file and resets
  fseek(file_p, 0, SEEK_END);
  content_length = ftell(file_p);
  fseek(file_p, 0, SEEK_SET);
  printf("content-length %d\n", content_length);
  
  // bottle neck of reading is min(fileSize, bufferSize)
  if (content_length > length) {
    content_length = length;
  }
  
  fread(*html, content_length, 1, file_p);
  //  fwrite(*html, content_length, 1, stdout);
  fclose(file_p);
  return content_length;
}
