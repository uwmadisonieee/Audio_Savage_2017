#include "server/server.h" 
#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h> // for sleep()

#define  LED0   0
#define  LED1   1
#define  LED2   2
#define  LED3   3

// vvv Callback Functions vvv
void onKeyPress( char key ) {
  printf("Key Pressed: %c\n", key);
}

void newSong( char* song ) {
  printf("New song selected: %s\n", song);
}

void toggleGPIO( int GPIO ) {
  digitalWrite(GPIO, !digitalRead(GPIO));
}
// ^^^ Callback Functions ^^^

int main ( int argc, char* argv[] ) {

  FILE* temperature_file;
  double current_temperature;
  char* thermal_path = "/sys/class/thermal/thermal_zone0/temp";

  server_t* server_config = (server_t*)setupServer();
  
  // set port of server
  server_config->port = 8000;
  
  // set function to receive a key press
  server_config->onKeyPress = onKeyPress;

  // set function to receive a song selection
  server_config->onSongSelect = newSong;

  // set functon to receive on GPIO toggle
  server_config->onGPIO = toggleGPIO;

  // start server
  startServer();
 
  // setup wiring Pin setup
  if (wiringPiSetup() == -1) {
          printf("setup wiringPi failed !\n");
          return 1;
  }

  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

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
    broadcastNumber("temperature", current_temperature);
    freopen(thermal_path, "r", temperature_file);
    sleep(1); // waits 1 second before probing again
  }
}
 