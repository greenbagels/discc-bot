/* @file bot.c
 * @author Sameed Pervaiz (greenbagels)
 * @short Core bot functionality
 * @copyright GPL v3
 */

// #include "./cJSON/cJSON.h"

/* For the initial HTTP requests */
#include <curl/curl.h>

/* For the WebSocket-based API */
// #include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"

struct received_data
{
	char *data;
	size_t size;
};

int initialize_data_struct(struct received_data *data);
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

int main(int argc, char *argv[])
{
	/* our initial pass on this code will be setting up a minimum working example;
	 * after that, we'll focus on turning this into a robust library! */

	/* ok i totally didn't mean we're gonna get it working and pretend in a week
	 * that this was never written, haha, yeah...  */

	const char *api_url = "https://discordapp.com/api/gateway";
	curl_global_init(CURL_GLOBAL_SSL);
	CURL *curl = curl_easy_init();
	struct received_data inc_data;
	initialize_data_struct(&inc_data);
	// TODO: handle malloc(1) error (lol)
	if (curl)
	{
		// error checking is for bitches
		// CURLcode result = 0;
		curl_easy_setopt(curl, CURLOPT_URL, api_url);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &inc_data);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		curl_easy_perform(curl);
		printf("%256s\n", inc_data.data);
		curl_easy_cleanup(curl);
	}
	// TODO: else {}
	
	return 0;
}

int initialize_data_struct(struct received_data *data)
{
	data->data = (char*)malloc(1);
	if (data->data == NULL)
	{
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
	struct received_data *persist = (struct received_data*) userdata;
	size_t size_delta = size * nmemb;
	if (realloc(persist->data, persist->size + size_delta) == NULL)
	{
		fprintf(stderr, "Reallocation of storage buffer failed!\n");
		return 0; // this should trigger a CURLE_WRITE_ERROR, i think
	}
	memcpy(persist->data + persist->size - 1, ptr, size_delta);
	persist->size += size_delta;
	persist->data[persist->size - 1] = 0;
	// how should we handle null terminators? at the end?? we'd need to keep a constant extra 1 len
	return size_delta;
};


