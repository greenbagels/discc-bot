/* @file json.c
 * @author Sameed Pervaiz (greenbagels)
 * @short libwebsockets-based JSON parsing utilities
 * @copyright GPL v3
 */

#include "json.h"

signed char usr_lejp_callback (struct lejp_ctx *ctx, char reason)
{
	// struct user_data* data = (struct user_data*)string;
	// on the first run, this should contain NULL
	int size_delta;
	struct udata *data = (struct udata*)ctx->user;

	switch(reason)
	{
		case LEJPCB_CONSTRUCTED:
			break;

		case LEJPCB_DESTRUCTED:
			// if (ret_string) free(ret_string);
			break;
		
		case LEJPCB_START:
			lwsl_user("Starting JSON parsing\n");
			break;
		
		case LEJPCB_COMPLETE:
			lwsl_user("JSON parsing complete!\n");
			lwsl_user("The url is now %s at line 31\n", data->string);
			break;

		case LEJPCB_FAILED:
			lwsl_user("JSON parsing failed!\n");
			break;

		case LEJPCB_PAIR_NAME:
			break;

		case LEJPCB_VAL_TRUE:
			// fall through

		case LEJPCB_VAL_FALSE:
			// fall through

		case LEJPCB_VAL_NULL:
			// fall through
			//
		case LEJPCB_VAL_NUM_INT:
			// fall through

		case LEJPCB_VAL_NUM_FLOAT:
			// string = strdup(ctx->buf);
			strcpy(data->string, ctx->buf);
			break;

		case LEJPCB_VAL_STR_START:
			lwsl_user("String parsing starting...\n");
			data->string = malloc(1);
			data->string[0] = 0;
			data->size = 0;
			memset(ctx->buf, 0, LEJP_STRING_CHUNK);
			break;

		case LEJPCB_VAL_STR_CHUNK:
			lwsl_user("Read a string chunk!\n");
			size_delta = LEJP_STRING_CHUNK-1;
			goto update_buffers;

		case LEJPCB_VAL_STR_END:
			lwsl_user("Finished a string!\n");
			size_delta = ctx->npos;
			goto update_buffers;

			update_buffers:
			data->string = realloc(data->string, data->size + size_delta);
			memcpy(data->string + data->size, ctx->buf, size_delta);
			data->size += size_delta;
			data->string[data->size] = 0;
			break;

		case LEJPCB_ARRAY_START:
			break;

		case LEJPCB_ARRAY_END:
			strcpy(data->string, ctx->path);
			break;

		case LEJPCB_OBJECT_START:
			lwsl_user("Object started!\n");
			break;

		case LEJPCB_OBJECT_END:
			lwsl_user("Object ended!\n");
			/* We want to handle nested JSON objects, but this WILL be called when the overall
			 * JSON object ends. So let's NOT enable this just quite yet.
			 *
			 * The best move overall, once we try seeing how the parser works with more complex
			 * JSON data, might be to make the user struct more modular (like a linked list)...
			 *
			 * strcpy(data->string, ctx->path);
			 */
			break;

		default:
			break;
	}
	return 0;
}

