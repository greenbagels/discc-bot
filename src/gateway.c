/* @file gateway.c
 * @author Sameed Pervaiz (greenbagels)
 * @short Discord Gateway Interface
 * @copyright GPL v3
 */

#define _POSIX_C_SOURCE 200809L
#include <libwebsockets.h>

#include <unistd.h>

#include "../inc/http.h"
#include "../inc/json.h"
#include "../inc/gateway.h"

char *get_gateway(const char* query_url)
{
	while (begin_http_session())
	{
		fprintf(stderr, "Failed to start http session. retrying in 5 sec...\n");
		sleep(5);
	}

	const char *json_data = http_get(query_url);
	//const char *json_data = "{\"foo\" : \"bar\", \"baz\" : {\"yeet\":\"skeet\"}, \"url\":\"dikdik\"}";
	struct lejp_ctx context;
	const char *names[] = {"url"}; // is this even useful?
	struct json_parse_data gateway_data;

	lejp_construct(&context, get_gateway_callback, &gateway_data, names, 1);
	while (lejp_parse(&context, (const unsigned char*)json_data, strlen(json_data)) < 0)
	{
		fprintf(stderr, "Incomplete parse... waiting for more input...\n");
		sleep(1);
		// let it finish parsing
	}
	lwsl_user("The string %s has length %lu\n", gateway_data.value, gateway_data.value_len);
	lejp_destruct(&context);
	end_http_session();
	return gateway_data.value;
}

signed char get_gateway_callback (struct lejp_ctx *ctx, char reason)
{
	// struct user_data* data = (struct user_data*)string;
	// on the first run, this should contain NULL
	int size_delta;
	struct json_parse_data *json_data = (struct json_parse_data*)ctx->user;

	switch(reason)
	{
		case LEJPCB_CONSTRUCTED:
			lwsl_debug("Parsing construct started\n");
			json_data->depth = 0; // this is our object nest level
			json_data->flag = 0; // user-defined flag

			json_data->name = NULL;
			json_data->value = NULL; // this could hold any json data type
			json_data->value_len = 0;

			break;

		case LEJPCB_DESTRUCTED:
			lwsl_debug("Parsing context destroyed\n");
			// if (ret_string) free(ret_string);
			break;
		
		case LEJPCB_START:
			lwsl_debug("Starting JSON parsing\n");
			break;
		
		case LEJPCB_COMPLETE:
			lwsl_debug("JSON parsing complete!\n");
			lwsl_debug("The url is now %s at line 31\n", json_data->value);
			break;

		case LEJPCB_FAILED:
			lwsl_debug("JSON parsing failed!\n");
			break;

		case LEJPCB_PAIR_NAME:
			lwsl_debug("Read key name\n");
			json_data->flag = 1; // now we care about storing the string
			break;

		case LEJPCB_VAL_STR_START:
			lwsl_debug("String parsing starting...\n");
			if (json_data->flag)
			{
				json_data->value = malloc(1);
				json_data->value[0] = 0;
				// this shouldn't be necessary, since lejp zero-terminates buff, right?
				memset(ctx->buf, 0, LEJP_STRING_CHUNK);
			}
			break;

		case LEJPCB_VAL_STR_CHUNK:
			lwsl_debug("Read a string chunk!\n");
			if (json_data->flag)
			{
				size_delta = LEJP_STRING_CHUNK-1;
				goto update_buffers;
			}
			break;

		case LEJPCB_VAL_STR_END:
			lwsl_debug("Finished a string!\n");
			if (json_data->flag)
			{
				size_delta = ctx->npos;
				goto update_buffers;
			}
			break;

		update_buffers:
			json_data->value = realloc(json_data->value, json_data->value_len + size_delta);
			memcpy(json_data->value + json_data->value_len, ctx->buf, size_delta);
			json_data->value_len += size_delta;
			json_data->value[json_data->value_len] = 0;
			break;

		default:
			break;
	}
	return 0;
}
