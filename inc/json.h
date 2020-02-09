/* @file json.h
 * @author Sameed Pervaiz (greenbagels)
 * @short libwebsockets-based JSON parsing utilities header
 * @copyright GPL v3
 */
#ifndef JSON_H
#define JSON_H

#ifndef _POSIX_C_SOURCE
	#define _POSIX_C_SOURCE 200809L
#endif

/* For the libwebsocket-based API */
// #include <pthread.h>
#include <libwebsockets.h>

struct json_parse_data
{
	// First, parsing data we want to keep persistent between calls (consider making these static instead?)
	char depth; // this is our object nest level
	char flag; // user-defined flag

	char* name;
	char* value; // this could hold any json data type
	size_t value_len;
};

signed char lejp_main_callback (struct lejp_ctx *ctx, char reason);

#endif
