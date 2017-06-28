#ifndef __SERVER_UTIL_H__
#define __SERVER_UTIL_H__

#include <sys/types.h> 			/* socket, setsockopt, accept, send, recv */
#include <sys/socket.h> 		/* socket, setsockopt, inet_ntoa, accept */
#include <netinet/in.h> 		/* sockaddr_in, inet_ntoa */
#include <arpa/inet.h> 			/* htonl, htons, inet_ntoa */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define WEBSITE_FOLDER "website"
#define PRINT_ERRORS 1

// Size used when allocating memory
#define MAX_RESPONSE_SIZE 5000000 // 5MB
#define MAX_HEADER_SIZE   20000   // 20kb
#define MAX_REQUEST_SIZE  500000  // 500KB

// function callback types
typedef void (*callback)(char*);
typedef void (*callbackChar)(char);
typedef void (*callbackInt)(int*);

typedef enum {
  GET,
  POST,
  UKKNOWN
} VERB;

typedef enum {
  HTTP,
  WEBSOCKET,
  API,
  UNKNOWN
} request_type;

// Info found in a WS and HTTP header
typedef struct {
  VERB verb;
  request_type type;
  int ws_version;
  char* route;
  char* ws_key;
  char* upgrade;
  char* client_ip;
} request_header;

// Info for each ws thread created
typedef struct ws_client_n {
  int socket_fp;
  pthread_t thread_id;
  request_header* header;
  ws_message* message;
  struct ws_client_n* next;
} ws_client;

typedef struct {
  char opcode[1];
  char mask[4];
  uint64_t len;
  uint64_t enc_len; 
  uint64_t next_len;
  char *msg;
  char *next;
  char *enc;
} ws_message;

// We need a list to hold all the current WS clients
typedef struct {
  int len;
  ws_client *first;
  ws_client *last;	
  pthread_mutex_t lock;
} ws_list;

// Configuration info sent from server
typedef struct server_t {
  int          port;
  callback     response;
  callbackChar onKeyPress;
} server_t;

//////////////////////////////////////////
////////////    Functions    /////////////
//////////////////////////////////////////

// makes error 1 liners
int printError(char* message,
	       int   return_val);

// makes error 1 line with returning NULL
void* printErrorNull(char* message);

// prints before exiting program
void printFatal(char* message);

// Takes a string and length and sets it to RFC 1123 Date Format for HTTP Response
void getTime(char** timestamp,
	     int    length);

// List functions.
ws_list* listNew(void);
void listFree(ws_list* list);
void listAdd(ws_list* list, ws_client* node);
void listRemove(ws_list* list, ws_client* remove);
void listRemoveAll(ws_list* list);
void listPrint(ws_list* list);
void listMulticastAll(ws_list* list, ws_message* message);

// Websocket functions.
void wsCloseframe(ws_client* client);
void wsSend(ws_client* client, ws_message* message);

// Memory handling functions
char* getMemoryChar(char* token, int length);

// New structures.
ws_client* client_new(int sock);
request_header* headerNew();
ws_message* messageNew();

// Free structures
void headerFree(request_header* header);
void messageFree(ws_message* message);
void clientFree(ws_client* client);

#endif
