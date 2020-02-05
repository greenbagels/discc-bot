/* @file http.c
 * @author Sameed Pervaiz (greenbagels)
 * @short libcurl-based HTTP utilities
 * @copyright GPL v3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"

int begin_http_session()
{
	//we REQUIRE ssl for our sessions
	CURLcode result = curl_global_init(CURL_GLOBAL_SSL);
	if (result)
	{
		// TODO: plan out the handling of errors in a more robust way; maybe our own list
		// of error codes?
		fprintf(stderr,"Initialization failed!\n");
		return 1;
	}
	return 0;
}

void end_http_session()
{
	curl_global_cleanup();
}

const char *http_get(const char *api_url)
{
	CURL *curl = curl_easy_init();
	struct GET_data inc_data;

	if(initialize_data_struct(&inc_data))
	{
		fprintf(stderr, "malloc(1) failed in initialize_data_struct!\n");
		return NULL;
	}

	if (curl)
	{
		// error checking is for bitches
		// CURLcode result = 0;
		curl_easy_setopt(curl, CURLOPT_URL, api_url);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &inc_data);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return inc_data.string;
	}
	else
	{
		fprintf(stderr, "curl_easy_init failed!\n");
		return NULL;
	}
}

int initialize_data_struct(struct GET_data *data)
{
	data->string = (char*)malloc(1);
	if (data->string == NULL)
	{
		data->size = 0;
		return 1;
	}
	data->size = 1;
	return 0;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	/* We can't be sure if the data being received is the entire message, or even any
	 * specific part of it. So let's keep concatenating it to the data in userdata, which is
	 * dynamically allocated (to avoid stack overflows).
	 */

	// god i wish i weren't using C right now
	struct GET_data *persist = (struct GET_data*) userdata;
	size_t size_delta = size * nmemb;
	persist->string = realloc(persist->string, persist->size + size_delta);
	if (persist->string == NULL)
	{
		fprintf(stderr, "Reallocation of storage buffer failed!\n");
		return 0; // this should trigger a CURLE_WRITE_ERROR, i think
	}
	memcpy(persist->string + persist->size - 1, ptr, size_delta);
	persist->size += size_delta;
	// Make sure to null terminate!
	persist->string[persist->size - 1] = 0;
	return size_delta;
}

