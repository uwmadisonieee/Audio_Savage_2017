#ifndef __API_H__
#define __API_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

// takes route and finds where to route it
// returns positive if success
//         0 if no route found
//         negative if error
int callApiRoute(char*     route,
		 char**    http_request,
		 HTTP_VERB verb);

#endif
