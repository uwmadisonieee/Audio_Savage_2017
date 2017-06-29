#include "server/server.h" 

void onResponse( char* response ) {
  printf("CALLBACK: %s\n", response);
}

void onKeyPress( char key ) {
  printf("Key Pressed: %c\n", key);
}

int main ( int argc, char* argv[] ) {

  int status;
  server_t server_config;
  char* data;
  
  // set port of server
  server_config.port = 8000;
  
  // set function to receive actions
  server_config.response = onResponse;

  // set function to receive a key press
  server_config.onKeyPress = onKeyPress;

  // start server
  status = server(&server_config);

  if (status < 0) {
    printf("http Server didn't start up correctly\n");
    exit(1);
  }
  
  while(1){
    printf("What is your biggest secret ever\n");
    scanf("%s", data);
    broadcast(data);    
  }
}
