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

struct websock_data
{
	int foo;
};

static struct lws_context *ws_ctx;
static struct lws *cwsi;
static const int port = 443;
static char addr[64];
static const char *proto;
static const char *path = "/?v=6&encoding=json";

signed char usr_lejp_callback (struct lejp_ctx *ctx, char reason);
int lws_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
int connect_sock(void);

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
	/* We're gonna use libwebsocket's JSON parser to cut down on libs we pull in (and build time)
	 * we can technically use LWS for our HTTP(S) API too, I think... */
	struct lejp_ctx context;
	const char *names[] = {"url"};
	struct user_data data;
	lejp_construct(&context, usr_lejp_callback, &data, names, 1);
	while (lejp_parse(&context, (const unsigned char*)string, strlen(string)) < 0)
	{
		fprintf(stderr, "Incomplete parse... waiting for more input...\n");
		sleep(1);
		// let it finish parsing
	}
	// TODO: change all these dynamically allocated arrays to const
	// length; our strings don't really need to be dynamically
	// allocated, and it makes security/failure a bigger concern,
	// i think...
	// const char *tmp = "/?v=6&encoding=json";
	// data.string = realloc(data.string, data.size + strlen(tmp) + 1);
	// data.size += strlen(tmp);
	// strncat(data.string, tmp, strlen(tmp));
	printf("%s\n", data.string);
	// strip out "wss://", so the first 6 bytes
	memcpy(addr, data.string+6, data.size-5);

	lejp_destruct(&context);
	/* but boy is it a lot uglier */
	/* now it's lws time */

	lws_set_log_level(LLL_USER | LLL_DEBUG | LLL_INFO | LLL_ERR | LLL_WARN | LLL_NOTICE, NULL);

	struct lws_context_creation_info info;

	struct lws_protocols protocols[] =
	{
		{ "discord-stuff", lws_callback, sizeof(struct websock_data),  0, },
		{ NULL, NULL, 0, 0 }
	};

	memset(&info, 0, sizeof(info));
	info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;

	proto = "discord-stuff";

	ws_ctx = lws_create_context(&info);
	if (!ws_ctx)
	{
		lwsl_err("lws init failed\n");
	}

	int n = 0;
	while (n >=0)
	{
		n = lws_service(ws_ctx, 1000);
	}

	if (string) free(string);
	end_http_session();
	return 0;
}

signed char usr_lejp_callback (struct lejp_ctx *ctx, char reason)
{
	struct user_data* data = (struct user_data*)ctx->user;
	int size_delta;
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
			data->string[0] = 0;
			data->size = 0;
			memset(ctx->buf, 0, LEJP_STRING_CHUNK);
			break;

		case LEJPCB_VAL_STR_CHUNK:
			size_delta = LEJP_STRING_CHUNK-1;
			goto update_buffers;

		case LEJPCB_VAL_STR_END:
			size_delta = strlen(ctx->buf);

		update_buffers:
			data->string = (char*)realloc(data->string, data->size + size_delta + 1);
			memcpy(data->string + data->size, ctx->buf, size_delta);
			data->size += size_delta;
			data->string[data->size] = 0;
			memset(ctx->buf, 0, size_delta);
			break;

		case LEJPCB_COMPLETE:
			break;

		default:
			break;
	}
	return 0;
}

struct msg { void *payload; size_t len; char binary; char first; char final; };

int lws_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{

	struct msg amesg;

	switch(reason)
	{
		case LWS_CALLBACK_PROTOCOL_INIT:
			if (connect_sock())
			{
				lwsl_user("Connected to WebSocket server!\n");
			}
			break;

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			break;

		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			lwsl_user("Client established!\n");
			break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:
			break;

		case LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL:
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
			break;

		case LWS_CALLBACK_TIMER:
			break;

		case LWS_CALLBACK_USER:
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:
			lwsl_user("Received data: %s\n", (char*)in);
			break;

		default:
			break;
	}

	return lws_callback_http_dummy(wsi, reason, user, in, len);
}

int connect_sock(void)
{
	struct lws_client_connect_info i;

	memset(&i, 0, sizeof(i));

	i.context = ws_ctx;
	i.port = port;
	i.address = addr;
	i.path = path;
	i.host = i.address;
	i.origin = i.address;
	i.ssl_connection = LCCSCF_USE_SSL;
	i.protocol = proto;
	i.local_protocol_name = "discord-stuff";
	i.pwsi = &cwsi;

	lwsl_user("Connecting to %s:%d%s\n", i.address, i.port, i.path);

	return  !lws_client_connect_via_info(&i);
}
