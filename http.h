/* @file http.h
 * @author Sameed Pervaiz (greenbagels)
 * @short libcurl-based HTTP utilities
 * @copyright GPL v3
 */

#ifndef HTTP_H
#define HTTP_H

/* For the initial HTTP requests */
#include <curl/curl.h>

struct GET_data
{
	char *string;
	size_t size;
};

int begin_http_session();
void end_http_session();
char *http_get(const char *api_url);
int initialize_data_struct(struct GET_data *data);
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

#endif
