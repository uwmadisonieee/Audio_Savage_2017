#ifndef __SERVER_UTIL_H__
#define __SERVER_UTIL_H__

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define WEBSITE_FOLDER "website"
#define PRINT_ERRORS 1

// makes error 1 liners
int printError(char* message,
	       int   return_val);

// Takes a string and length and sets it to RFC 1123 Date Format for HTTP Response
void getTime(char** timestamp,
	     int    length);

// Takes files and sets it right align with the memeory at
// return_body of size length
// Returns the length of content length or -1 if error
int getFileContent(char* relative_path,
		   char* return_body,
		   int   length);

#endif
