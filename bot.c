/* @file bot.c
 * @author Sameed Pervaiz (greenbagels)
 * @short Core bot functionality
 * @copyright GPL v3
 */

#define _POSIX_C_SOURCE 200809L
/* For the WebSocket-based API */
#include <pthread.h>
#include <libwebsockets.h>

/* Language level headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Platform-level headers */
#include <unistd.h>

// #include "./cJSON/cJSON.h"

#include "http.h"

struct user_data
{
	char *string;
	size_t size;
};

signed char callback (struct lejp_ctx *ctx, char reason);

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
	// at this point, we need to parse some JSON
	// cJSON *json = cJSON_Parse(string);
	// const cJSON *json_url = cJSON_GetObjectItem(json, "url");
	// printf("%s\n", cJSON_Print(json_url));
	/* We're gonna use libwebsocket's JSON parser to cut down on libs we pull in (and build time) */
	struct lejp_ctx context;
	const char *names[1] = {"url"};
	struct user_data data;
	lejp_construct(&context, callback, &data, names, 1);
	while (lejp_parse(&context, (const unsigned char*)string, strlen(string)) < 0)
	{
		fprintf(stderr, "Incomplete parse... waiting for more input...\n");
		sleep(1);
		// let it finish parsing
	}
	printf("%s\n", data.string); 
	lejp_destruct(&context);
	/* but boy is it a lot uglier */

	if (string) free(string);
	end_http_session();
	return 0;
}

signed char callback (struct lejp_ctx *ctx, char reason)
{
	struct user_data* data = (struct user_data*)ctx->user;
	switch(reason)
	{
		case LEJPCB_CONSTRUCTED:
			// data->string = (char*)malloc(1);
			// data->size = 1;
			break;

		case LEJPCB_DESTRUCTED:
			free(data->string);
			break;

		case LEJPCB_VAL_STR_START:
			data->string = (char*)malloc(1);
			data->size = 1;
			memset(ctx->buf, 0, LEJP_STRING_CHUNK);
			break;

		case LEJPCB_VAL_STR_CHUNK:
			data->string = (char*)realloc(data->string, data->size + LEJP_STRING_CHUNK - 1);
			memcpy(data->string + data->size - 1, ctx->buf, LEJP_STRING_CHUNK - 1);
			data->size += LEJP_STRING_CHUNK - 1;
			data->string[data->size - 1] = 0;
			memset(ctx->buf, 0, LEJP_STRING_CHUNK);
			break;

		case LEJPCB_VAL_STR_END:
			// not sure whether there's a stored value for the amount of bytes stored...
			data->string = (char*)realloc(data->string, data->size + strlen(ctx->buf));
			memcpy(data->string + data->size - 1, ctx->buf, strlen(ctx->buf));
			data->size += LEJP_STRING_CHUNK - 1;
			data->string[data->size - 1] = 0;
			memset(ctx->buf, 0, LEJP_STRING_CHUNK);
			break;

		case LEJPCB_COMPLETE:
			break;

		default:
			break;
	}
	return 0;
}
