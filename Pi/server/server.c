#include "server.h"

pthread_t server_thread;

static int status; // used to check status of c functions return values

int server(server_t* config) {

  if (config->port <= 1024 || config->port >= 65536) {
    return printError("Port must be between 1024 and 65536\n", -1);
  }

  status = pthread_create(&server_thread,
			  NULL,
			  serverDaemon,
			  (void *)config);
  
  // Daemon tread_create status is what determines success
  return status;
}

void broadcast(char* message)
{
  //todo
}

void* serverDaemon(void *config) {

  //--------------------------------//
  //         Variable Setup         //
  //--------------------------------//
  
  int port = ((server_t*)config)->port;
  int status;
  int on = 1;
  
  char* request_HTTP = malloc(MAX_REQUEST_SIZE);
  int   request_size;
  char* header_temp = malloc(MAX_HEADER_SIZE); //TODO not limit
  
  //--------------------------------//
  //       Configure TCP Socket     //
  //--------------------------------//
  
  struct sockaddr_in client;    // socket info about the machine connecting to us
  struct sockaddr_in server;    // socket info about our server
  int socket_fp;                // socket used to listen for incoming connections
  int socket_con;               // used to hold status of connect to socket
  socklen_t socket_size = sizeof(struct sockaddr_in);

  memset(&server, 0, sizeof(server));          // zero the struct before filling the fields
  server.sin_family = AF_INET;                 // set to use Internet address family
  server.sin_addr.s_addr = htonl(INADDR_ANY);  // sets our local IP address
  server.sin_port = htons(port);               // sets the server port number

  // creates the socket
  // AF_INET refers to the Internet Domain
  // SOCK_STREAM sets a stream to send data
  // 0 will have the OS pick TCP for SOCK_STREAM
  socket_fp = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fp < 0) {
    printf("ERROR: Opening socket\n");
    pthread_exit(NULL);
  }
  else {
    printf("TCP Socket Created!\n");
  }

  // This prevents the TIME_WAIT socket error on reloading
  status = setsockopt(socket_fp, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  if (status < 0) {
    printf("ERROR: setting SOL_SOCKET\n");
    pthread_exit(NULL);
  }

  // bind server information with server file poitner
  status = bind(socket_fp, (struct sockaddr *)&server, sizeof(struct sockaddr));

  // checks for TIME_WAIT socket
  // when daemon is closed there is a delay to make sure all TCP data is propagated
  if (status < 0) {
    printf("ERROR opening socket: %d , possible TIME_WAIT\n", status);
    printf("USE: netstat -ant|grep %d to find out\n", port);
    pthread_exit(NULL);
  } else {
    printf("Socket Binded!\n");
  }

  // start listening, allowing a queue of up to 10 pending connection
  listen(socket_fp, 10);
  printf("Socket Listening on port %d!\n\n", port);

  //--------------------------------//
  //         Server Polling         //
  //--------------------------------//
  
  while(1) {

    //blocking for response
    socket_con = accept(socket_fp, (struct sockaddr *)&client, &socket_size);

    // inet_ntoa(client.sin_addr)
    
    request_size = recv(socket_con, request_HTTP, MAX_REQUEST_SIZE, 0);

    request_header* header = parseHeader(&request_HTTP);
    if (NULL == header) {
      printf("Couldn't allocate header!");
    }
    
    if ( WEBSOCKET == header.type ) {
      wsHandle(config);
    } else if ( HTTP == header.type ) {
      httpHandle(config, socket_con);
    }

    //printf("waiting for next request\n");
    //printf("--------------------------\n");
    socket_con = accept(socket_fp, (struct sockaddr *)&client, &socket_size);
  }

  pthread_exit(NULL); 
}

request_header* parseHeader(char** request)
{

  // Defaults are set which we used to infer it wasn't found in header
  request_header* header = headerNew();

  char* token = strtok(*request, "\r\n");
  char* route;
  int route_length = 0;
  
  if (token != NULL) {

    // first check for Verb
    if ( 0 == strncasecmp("GET /", token, 5)) {
      header->verb = GET;
    } else if ( 0 == strncasecmp("POST /", token, 6)) {
      header->verb = POST;
    } else {
      return printErrorNull("Not Get or Post");
    }

    route = strstr(*request, "/");
    // finds where route path ends
    while (route[route_length] != ' ') {
      route_length++;
    }
    strncpy(header->route, route, route_length);
    
    // time to loop through header lines
    while ( token != NULL ) {

      if (0 == strncasecmp("Sec-WebSocket-Version: ", token, 23)) {
	header->ws_version = strtol(token+23, (char**) NULL, 10);
      } else if (0 == strncasecmp("Sec-WebSocket-Key: ", token, 19)) {
	header->ws_key = token + 19;
      } else if (0 == strncasecmp("Upgrade: ", token, 9)) {
	header->upgrade = token + 9;
      }
      
      token = strtok(NULL, "\r\n");
    }

    // time to validate and determine what we were requested
    if ( 0 == header->ws_version ) {
      // HTTP
      header->type = HTTP;
      return header;
      
    } else if ( 13 == header->ws_version ) {
      
      if ( 0 == strncasecmp(header->upgrade, "websocket", 9) &&
	   (NULL != header->upgrade) && (NULL != header->ws_key)) {
	// websocket RFC6455
	header->type = WEBSOCKET;
	return header;
	
      }	else {
	return printErrorNull("Need Socket upgrade and key in header");
      }
    } else {
      return printErrorNull("Only RFC6455 Websockets supported");
    }	
  } else {
    return printErrorNull("Parse Header Error!");
  }  
}
