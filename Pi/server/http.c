#include "http.h"

pthread_t server_thread;

static int status; // used to check status of c functions return values

int httpServer(http_t *http) {

  if (http->port <= 1024 || http->port >= 65536) {
    printf("Port must be between 1024 and 65536\n");
    return -1;
  }
  
  status = pthread_create(&server_thread,
			  NULL,
			  httpDaemon,
			  (void *)http);
  
  // Daemon tread_create status is what determines success
  return status;
}

/*
  Creates a block of memory of MAX_HTTP_SIZE large and saves a
  part of it for header. The body gets put in the body section
  right aligned to the buffer. The header section is inserted at
  the right in front of the body. This means there will be a gap
  between the beginning of the buffer and where the head start
  which is the part that gets sent back to client
 */
void* httpDaemon(void *config) {

  //--------------------------------//
  //         Variable Setup         //
  //--------------------------------//
  
  int port = ((http_t*)config)->port;
  int status;
  int on = 1;
  
  char* request_HTTP = malloc(MAX_REQUEST_SIZE);
  int   request_size;
  char* response_HTTP = malloc(MAX_RESPONSE_SIZE + 2);
  char* header_temp = malloc(MAX_HEADER_SIZE); //TODO not limit
  char* route = malloc(sizeof(char) * 256); // TODO limit?
  char* timestamp = malloc(sizeof(char) * 256);
  int   content_length;
  int   header_offset;
  int   header_length;
  
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
  else { printf("TCP Socket Created!\n"); }

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
  //blocking for response
  socket_con = accept(socket_fp, (struct sockaddr *)&client, &socket_size);

  //--------------------------------//
  //      HTTP Server Polling       //
  //--------------------------------//
  
  while(socket_con) {

    request_size = recv(socket_con, request_HTTP, MAX_REQUEST_SIZE, 0);

    //printf("HTTP Request Size: %d\n", request_size);

    //printf("\n%s\n", request_HTTP);
    
    // inet_ntoa(client.sin_addr) == string of sender IP;
   
    // Get Route since we need if not API and need file path
    findRoute(&request_HTTP, &route);

    printf("Route: %s\n", route);
    
    /////////////////////////////////////////
    // HTTP Reponse - Need to format string//
    /////////////////////////////////////////

    status = callApiRoute(&request_HTTP, &response_HTTP, route, (http_t*)config);

    if (0 == status) {
      // Not a API route, checking for file
      
      // generates timestamp for response header
      getTime(&timestamp, 256);

      // gets contents from file to send back in response boday
      content_length = getFileContent(route,
				      &response_HTTP,
				      MAX_RESPONSE_SIZE);

      if (content_length < 0) {
	sprintf(response_HTTP, "HTTP/1.1 400 OK\r\nCache-Control: no-cache, private\r\nDate: %s\r\n\r\n", timestamp);
	header_length = strlen(response_HTTP);
	header_offset = 0;
	content_length = 0; // need for send() logic
      } else {
	// gets a pointer where the response_body starts
	sprintf(header_temp, "HTTP/1.1 200 OK\r\nCache-Control: no-cache, private\r\nContent-Length: %i\r\nDate: %s\r\n\r\n", content_length, timestamp);
	header_length = strlen(header_temp);

	if (content_length + header_length > MAX_RESPONSE_SIZE) {
	  // TODO - too large of response
	}
	
	// offset where header is from start of buffer
	header_offset = MAX_RESPONSE_SIZE - content_length - header_length;

	memcpy(response_HTTP + header_offset, header_temp, header_length);
      }
      
    } else if (status > 0) {
      // api was called
      sprintf(response_HTTP, "HTTP/1.1 200 OK\r\nCache-Control: no-cache, private\r\nContent-Length: 11\r\nContent-Type: application/json\r\nDate: Sat, 24 Jun 2017 05:29:07\r\n\r\n{\"test\":42}\r\n");
      header_length = strlen(response_HTTP);
      header_offset = 0;
      content_length = 0; // need for send() logic
     } else {
      //tODO error
    }
    
    send(socket_con, response_HTTP + header_offset, header_length + content_length, 0);

    close(socket_con);

    memset(request_HTTP, 0, request_size); //clears request message
	
    //printf("waiting for next request\n");
    //printf("--------------------------\n");
    socket_con = accept(socket_fp, (struct sockaddr *)&client, &socket_size);
  }

  pthread_exit(NULL);
  
}

void findRoute(char** request, char** route) {
    int end_index = 0;
    char *route_str;

    //finds when the route starts
    route_str = strstr(*request, "/");

    //finds when the route ends
    while (route_str[end_index] != ' ') {
        end_index++;
    }

    //copies route from message to reference string
    strncpy(*route, route_str, end_index);

    //ends the string so if a shorter string is passed then last route otherwise previous longer strings will remain
    strcpy(*route + end_index, "\0");

}
