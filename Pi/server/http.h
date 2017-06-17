#ifndef __HTTP_H__
#define __HTTP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <pthread.h>

#include "util.h"
#include "api.h"

// saves a chunk of memory for header
#define MAX_RESPONSE_SIZE 5000000 // 5MB
#define MAX_HEADER_SIZE 20000   // 20kb

#define MAX_REQUEST_SIZE 500000  // 500KB

// Call to spin up an HTTP sever
// Returns 0 on success
int httpServer(http_t *http);

// Creates a new thread to run the Daemon server
static void* httpDaemon(void *config);

// Parses out route from a HTTP request string
static void findRoute(const char* request, char** route);

#endif
