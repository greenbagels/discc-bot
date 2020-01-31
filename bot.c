/* @file bot.c
 * @author Sameed Pervaiz (greenbagels)
 * @short Core bot functionality
 * @copyright GPL v3
 */

/* For the WebSocket-based API */
// #include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

// #include "./cJSON/cJSON.h"
//
#include "http.h"

int main(int argc, char *argv[])
{
	/* our initial pass on this code will be setting up a minimum working example;
	 * after that, we'll focus on turning this into a robust library! */

	/* ok i totally didn't mean we're gonna get it working and pretend in a week
	 * that this was never written, haha, yeah...  */
	
	while (begin_http_session())
	{
		fprintf(stderr, "Failed to start http session. retrying in 5 sec...\n");
		sleep(5);
	}

	char *url = "https://discordapp.com/api/gateway";
	char *string = http_get(url);
	printf("%s\n", string);

	if (string) free(string);
	end_http_session();
	return 0;
}

