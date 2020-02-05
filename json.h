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

struct udata
{
	char* string;
	size_t size;
};

signed char usr_lejp_callback (struct lejp_ctx *ctx, char reason);

#endif
