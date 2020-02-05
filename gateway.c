/* @file gateway.c
 * @author Sameed Pervaiz (greenbagels)
 * @short Discord Gateway Interface
 * @copyright GPL v3
 */

#define _POSIX_C_SOURCE 200809L
#include <libwebsockets.h>

#include <unistd.h>

#include "http.h"
#include "json.h"

char *get_gateway(const char* query_url)
{
	while (begin_http_session())
	{
		fprintf(stderr, "Failed to start http session. retrying in 5 sec...\n");
		sleep(5);
	}

	const char *json_data = http_get(query_url);
	
	struct lejp_ctx context;
	const char *names[] = {"url"};
	struct udata data;

	lejp_construct(&context, usr_lejp_callback, &data, names, 1);
	while (lejp_parse(&context, (const unsigned char*)json_data, strlen(json_data)) < 0)
	{
		fprintf(stderr, "Incomplete parse... waiting for more input...\n");
		sleep(1);
		// let it finish parsing
	}
	lwsl_user("The string %s has length %lu\n", data.string, data.size);
	lejp_destruct(&context);
	end_http_session();
	return data.string;
}
