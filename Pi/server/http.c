#include "http.h"

/*
  Creates a block of memory of MAX_HTTP_SIZE large and saves a
  part of it for header. The body gets put in the body section
  right aligned to the buffer. The header section is inserted at
  the right in front of the body. This means there will be a gap
  between the beginning of the buffer and where the head start
  which is the part that gets sent back to client
 */
void* httpHandle(server_t *config) {

  int status;
  
  char* request_HTTP = malloc(MAX_REQUEST_SIZE);
  int   request_size;
  char* response_HTTP = malloc(MAX_RESPONSE_SIZE + 2);
  char* header_temp = malloc(MAX_HEADER_SIZE); //TODO not limit
  char* route = malloc(sizeof(char) * 256); // TODO limit?
  char* timestamp = malloc(sizeof(char) * 256);
  int   content_length;
  int   header_offset;
  int   header_length;
      
  // Get Route since we need if not API and need file path
  findRoute(&request_HTTP, &route);
  
  printf("Route: %s\n", route);
    
  // status = callApiRoute(&request_HTTP, &response_HTTP, route, (http_t*)config);

  // Not a API route, checking for file
      
  // generates timestamp for response header
  getTime(&timestamp, 256);

  // gets contents from file to send back in response boday
  content_length = getFileContent(route,
				  &response_HTTP,
				  MAX_RESPONSE_SIZE);

  if (content_length < 0) {
    sprintf(response_HTTP, "HTTP/1.1 400 OK\r\nCache-Control: no-cache, private\r\nDate: %s\r\n\r\n", timestamp);
    header_length = strlen(response_HTTP);
    header_offset = 0;
    content_length = 0; // need for send() logic
  } else {
    // gets a pointer where the response_body starts
    sprintf(header_temp, "HTTP/1.1 200 OK\r\nCache-Control: no-cache, private\r\nContent-Length: %i\r\nDate: %s\r\n\r\n", content_length, timestamp);
    header_length = strlen(header_temp);

    if (content_length + header_length > MAX_RESPONSE_SIZE) {
      // TODO - too large of response
    }
	
    // offset where header is from start of buffer
    header_offset = MAX_RESPONSE_SIZE - content_length - header_length;
    
    memcpy(response_HTTP + header_offset, header_temp, header_length);
  }
      
  // api was called
  //sprintf(response_HTTP, "HTTP/1.1 200 OK\r\nCache-Control: no-cache, private\r\nContent-Length: 11\r\nContent-Type: application/json\r\nDate: Sat, 24 Jun 2017 05:29:07\r\n\r\n{\"test\":42}\r\n");
  //header_length = strlen(response_HTTP);
  //header_offset = 0;
  //content_length = 0; // need for send() logic
  //} else {
  //  tODO error
  //}
  
  send(socket_con, response_HTTP + header_offset, header_length + content_length, 0);
  
  close(socket_con);
  
  memset(request_HTTP, 0, request_size); //clears request message
  
  return;
}

void findRoute(char** request, char** route) {
    int end_index = 0;
    char *route_str;

    //finds when the route starts
    route_str = strstr(*request, "/");

    //finds when the route ends
    while (route_str[end_index] != ' ') {
        end_index++;
    }

    //copies route from message to reference string
    strncpy(*route, route_str, end_index);

    //ends the string so if a shorter string is passed then last route otherwise previous longer strings will remain
    strcpy(*route + end_index, "\0");
}

int getFileContent(char* relative_path, char** return_body, int length) {

  FILE* file_p;
  int   content_length;
  int   front_offset = 0;
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
  } else {
    front_offset = length - content_length;
  }

  // reads in file so its right align with max length
  fread(*return_body + front_offset, content_length, 1, file_p);
  fclose(file_p);
  return content_length;
}
