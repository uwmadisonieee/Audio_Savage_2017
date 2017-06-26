#ifndef __SERVER_SERVER_H__
#define __SERVER_SERVER_H__

#include "util.h"
#include "http.h"
#include "websocket.h"

#include <pthread.h>

// Call to spin up an HTTP sever
// Returns 0 on success
int server(server_t *config);

// Used to broadcast to all websockets
void boradcast(char* message);

// Creates a new thread to run the Daemon server
static void* serverDaemon(void *config);

// takes request and parses header to struct
// returns 0 if valid, neg if error
static int parseHeader(
			char** request_all,
			request_header* header);

#endif
