//Use netstat -ant|grep <PORT> | awk '{print$6}'    -to check status of socket if in TIME_WAIT

#include "server/http.h" 

void onResponse( char* response ) {
  printf("CALLBACK: %s\n", response);
}

int main ( int argc, char* argv[] ) {

  int status;
  http_t http;

  // set port of server
  http.port = 8000;
  
  // set function to receive actions
  http.response = onResponse;

  // start server
  status = httpServer(&http);

  if (status < 0) {
    printf("http Server didn't start up correctly\n");
    exit(1);
  }
  
  while(1){}
}
