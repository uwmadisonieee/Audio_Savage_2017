#ifndef __SERVER_SERVER_H__
#define __SERVER_SERVER_H__

#include "util.h"
#include "http.h"
#include "websocket.h"

// sets default server_t and returns it back
server_t* setupSever(void);

// Call to spin up an HTTP sever
void startServer(void);

// Used to broadcast to all websockets
void broadcast(double temperature);

// Creates a new thread to run the Daemon server
void* serverDaemon();

// takes request and parses header to struct
// returns header if valid, NULL if error
request_header* parseHeader(char** request_all);

// Takes header and adds a valid SHA-1 key
void getSHA(request_header* header);

#endif
