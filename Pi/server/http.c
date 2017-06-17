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
  Creates a block of memory of MAX_HTTP_SIZE large and splits it
  into a header section and body section. The body gets put in the
  body section like normal, but the header section is inserted at
  the end of the header section so there will be a unused gap at
  beginning of header section, but this is so they align next to
  each other and there is no need to memcpy the memory more than once
 */
void* httpDaemon(void *config) {

  //--------------------------------//
  //         Variable Setup         //
  //--------------------------------//
  
  int port = ((http_t*)config)->port;

  char* receive_HTTP = malloc(MAX_HTTP_SIZE);
  int   receive_size;
  //  char* msg_callback = malloc(MAX_HTTP_SIZE);
  char* return_HTTP = malloc(MAX_RETURN_SIZE);
  //  char* return_body;
  char* header_temp = malloc(MAX_HEADER_SIZE);
  char* route = malloc(sizeof(char) * 128);
  char* timestamp = malloc(sizeof(char) * 256);
  int   content_length;
  //  int   header_offset;
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

    receive_size = recv(socket_con, receive_HTTP, MAX_HTTP_SIZE, 0);

    printf("HTTP Request Size: %d\n", receive_size);
    
    // inet_ntoa(client.sin_addr) == string of sender IP;

    /////////////////////////////////////////////////
    // HTTP Request - Need to handle it accordingly//
    /////////////////////////////////////////////////
    
    if (strncmp(receive_HTTP, "GET", 3) == 0) { //GET Request

      findRoute(receive_HTTP, &route);

      //      get_FindMethod(route, &returnMsg);

    } else if (strncmp(receive_HTTP, "POST", 4) == 0) { //POST Request

      findRoute(receive_HTTP, &route);
      //findBody(receive_HTTP, &postBody);
      //post_FindMethod(route, &returnMsg, postBody);

    } else { //something not GET or POST
      strcpy(return_HTTP, "HTTP Method not supported");
      // TODO
    }

    printf("Route: %s\n", route);
    
    memset(receive_HTTP, 0, receive_size); //clears receive message

    /////////////////////////////////////////
    // HTTP Reponse - Need to format string//
    /////////////////////////////////////////
    
    if (strncmp(route, "/key/", 5) == 0) {

      ((http_t*)config)->response(route + 5);
      strcpy(return_HTTP, "Key Received");

    } else {

      // generates timestamp for return header
      getTime(&timestamp, 256);

      // gets contents from file to send back in return boday
      content_length = getFileContent(route,
				      return_HTTP + MAX_HEADER_SIZE,
				      MAX_BODY_SIZE);

      if (content_length < 0) {
	sprintf(return_HTTP, "HTTP/1.1 400 OK\r\nCache-Control: no-cache, private\r\nDate: %s\r\n\r\n", timestamp);
	header_length = strlen(return_HTTP);
	//header_offset = 0;
	content_length = 0; // need for send() logic
      } else {
	// gets a pointer where the return_body starts
	//	return_body = return_HTTP + (MAX_BODY_SIZE - content_length);
	sprintf(header_temp, "HTTP/1.1 200 OK\r\nCache-Control: no-cache, private\r\nContent-Length: %i\r\nDate: %s\r\n\r\n", content_length, timestamp);
	header_length = strlen(header_temp);
	//header_offset = (HEADER_SIZE - header_length);
	//memcpy(return_HTTP + header_offset, header_temp, header_length);
	memcpy(return_HTTP + (MAX_RETURN_SIZE - content_length - header_length), header_temp, header_length);
      }
      
    }
    
    send(socket_con, return_HTTP + (MAX_RETURN_SIZE - content_length - header_length), header_length + content_length, 0);

    close(socket_con);

    //printf("waiting for next request\n");
    //printf("--------------------------\n");
    socket_con = accept(socket_fp, (struct sockaddr *)&client, &socket_size);
  }

  pthread_exit(NULL);
  
}

void findRoute(const char* request, char** route) {
    int end_index = 0;
    char *route_str;

    //finds when the route starts
    route_str = strstr(request, "/");

    //finds when the route ends
    while (route_str[end_index] != ' ') {
        end_index++;
    }

    //copies route from message to reference string
    strncpy(*route, route_str, end_index);

    //ends the string so if a shorter string is passed then last route otherwise previous longer strings will remain
    strcpy(*route + end_index, "\0");

}
