/* @file bot.c
 * @author Sameed Pervaiz (greenbagels)
 * @short Core bot functionality
 * @copyright GPL v3
 */

#define _POSIX_C_SOURCE 200809L
/* For the WebSocket-based API */
#include <libwebsockets.h>

/* Language level headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Platform-level headers */
#include <unistd.h>

// #include "./cJSON/cJSON.h"
// #include "bot.h"
#include "gateway.h"
#include "http.h"
#include "json.h"

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

int lws_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
int connect_sock(void);

int main(int argc, char *argv[])
{
	/* our initial pass on this code will be setting up a minimum working example;
	 * after that, we'll focus on turning this into a robust library! */

	/* ok i totally didn't mean we're gonna get it working and pretend in a week
	 * that this was never written, haha, yeah...  */

	lws_set_log_level(LLL_USER | LLL_DEBUG | LLL_INFO | LLL_ERR | LLL_WARN | LLL_NOTICE, NULL);
	const char *url = "https://discordapp.com/api/gateway";
	char *gateway_url = get_gateway(url);
	/* We're gonna use libwebsocket's JSON parser to cut down on libs we pull in (and build time)
	 * we can technically use LWS for our HTTP(S) API too, I think... */
	printf("The string \"%s\" has length %lu\n", gateway_url, strlen(gateway_url));
	// strip out "wss://", so the first 6 bytes
	memcpy(addr, gateway_url+6, strlen(gateway_url)-5);

	/* but boy is lws's json parser a lot more painful to use... */
	
	/* oh well, now it's lws time */

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
	// TODO: take winny's advice and use libev or something (thx winny!)
	while (n >=0)
	{
		n = lws_service(ws_ctx, 1000);
	}

	if (gateway_url) free(gateway_url);
	return 0;
}

int lws_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
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
