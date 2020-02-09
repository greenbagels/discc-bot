/* @file json.c
 * @author Sameed Pervaiz (greenbagels)
 * @short libwebsockets-based JSON parsing utilities
 * @copyright GPL v3
 */

#include "../inc/json.h"

// It seems that LEJP doesn't actually care what string we tell it we want
// so we should just make a full fledged parsing callback instead of our
// barebones one...

signed char lejp_main_callback (struct lejp_ctx *ctx, char reason)
{
	/* So basically we want to obtain opcode, payload, and optional
	 * data, and hand these off to the proper callback handlers
	 */
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
			break;

		case LEJPCB_FAILED:
			lwsl_debug("JSON parsing failed!\n");
			break;

		case LEJPCB_PAIR_NAME:
			lwsl_debug("Read key name\n");
			json_data->flag = 1;
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
			lwsl_debug("Read numerical literal!\n");
			// string = strdup(ctx->buf);
			strcpy(json_data->value, ctx->buf);
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
			if (!json_data->flag)
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

		case LEJPCB_ARRAY_START:
			lwsl_debug("Array started\n");
			break;

		case LEJPCB_ARRAY_END:
			lwsl_debug("Array ended\n");
			strcpy(json_data->value, ctx->path);
			break;

		case LEJPCB_OBJECT_START:
			lwsl_debug("Object started!\n");
			break;

		case LEJPCB_OBJECT_END:
			lwsl_debug("Object ended!\n");
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

