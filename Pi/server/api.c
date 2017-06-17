#include "api.h"

int callApiRoute(char** request_HTTP, char** response_HTTP, char* route, http_t* config)
{
  //  char* request_body;
  
  // Get HTTP Verb Type
  if (strncmp(*request_HTTP, "GET", 3) == 0) {

    if (strncmp(route, "/key/", 5) == 0) {
      config->response(route + 5);
      strcpy(*response_HTTP, "Key Received");
      return 1;
    }

  } else if (strncmp(*request_HTTP, "POST", 4) == 0) {

  } else {
    return printError("HTTP Verb not supported\n", -1);
  }


  // request_body = strstr(*http_request, "\n");
  
  return 0;
}
