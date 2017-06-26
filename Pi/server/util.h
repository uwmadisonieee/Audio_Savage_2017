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
  POST
} VERB;

typedef enum {
  HTTP,
  WEBSOCKET,
  API
} request_type;

// Info found in a WS and HTTP header
typedef struct {
  VERB verb;
  request_type type;
  int ws_version;
  char* ws_key;
  char* upgrade;
  char* client_ip;
} request_header;

// Info for each ws thread created
typedef struct {
  int socket_fp;
  pthread_t thread_id;
  request_header* header;
  char* message;
} ws_client;

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

// List functions.
ws_list *list_new(void);
ws_client *list_get(ws_list *l, char *addr, int socket);
void list_free(ws_list *l);
void list_add(ws_list *l, ws_client *n);
void list_remove(ws_list *l, ws_client *r);
void list_remove_all(ws_list *l);
void list_print(ws_list *l);
void list_multicast(ws_list *l, ws_client *n);
void list_multicast_one(ws_list *l, ws_client *n, ws_message *m);
void list_multicast_all(ws_list *l, ws_message *m);

// Websocket functions.
void ws_closeframe(ws_client *n, ws_connection_close c);
void ws_send(ws_client *n, ws_message *m);

// New structures.
ws_client *client_new(int sock, char *addr);
ws_header *header_new();
ws_message *message_new();

// Free structures
void header_free(ws_header *h);
void message_free(ws_message *m);
void client_free(ws_client *n);

// makes error 1 liners
int printError(char* message,
	       int   return_val);

// Takes a string and length and sets it to RFC 1123 Date Format for HTTP Response
void getTime(char** timestamp,
	     int    length);

#endif
