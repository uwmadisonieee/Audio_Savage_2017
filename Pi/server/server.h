#ifndef __SERVER_SERVER_H__
#define __SERVER_SERVER_H__

#include "util.h"
#include "http.h"
#include "websocket.h"

// Call to spin up an HTTP sever
// Returns 0 on success
int server(server_t *config);

// Used to broadcast to all websockets
void boradcast(char* message);

// Creates a new thread to run the Daemon server
static void* serverDaemon(void *config);

// takes request and parses header to struct
// returns header if valid, NULL if error
static request_header* parseHeader(char** request_all);

#endif
