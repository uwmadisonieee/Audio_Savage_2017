#include "server/server.h" 
#include <stdio.h>
#include <unistd.h> // for sleep()

void onKeyPress( char key ) {
  printf("Key Pressed: %c\n", key);
}

int main ( int argc, char* argv[] ) {

  FILE* temperature_file;
  double current_temperature;
  char* thermal_path = "/sys/class/thermal/thermal_zone0/temp";

  server_t* server_config = (server_t*)setupServer();
  
  // set port of server
  server_config->port = 8000;
  
  // set function to receive a key press
  server_config->onKeyPress = onKeyPress;

  // start server
  startServer();
 
  // Get temperature file from linux file system
  temperature_file = fopen(thermal_path, "r");
  if (NULL == temperature_file) {
    printf("No temperature file found on machine\n");
    exit(1);
  }

  printf("Probing temperature and broadcasting to sockets\n");
  while(1){
    fscanf(temperature_file, "%lf", &current_temperature);
    current_temperature /= 1000;
    broadcast(current_temperature);
    freopen(thermal_path, "r", temperature_file);
    sleep(1); // waits 1 second before probing again
  }
}
