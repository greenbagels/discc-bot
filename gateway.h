/* @file gateway.h
 * @author Sameed Pervaiz (greenbagels)
 * @short gateway-related interface header
 * @copyright GPL v3
 */

#ifndef GATEWAY_H
#define GATEWAY_H

enum gateway_opcode
{
	/* ---------- Gateway Opcodes -------- */
	GATEWAY_DISPATCH = 0,
	GATEWAY_HEARTBEAT = 1,
	GATEWAY_IDENTIFY = 2,
	GATEWAY_STATUS_UPDATE = 3,
	GATEWAY_VOICE_STATE_UPDATE = 4,
	GATEWAY_RESUME = 6,
	GATEWAY_RECONNECT = 7,
	GATEWAY_REQUEST_GUILD_MEMBERS = 8,
	GATEWAY_INVALID_SESSION = 9,
	GATEWAY_HELLO = 10,
	GATEWAY_HEARTBEAT_ACK = 11,
	/* ------ Gateway Close Opcodes ------ */
	GATEWAY_CLOSE_UNKNOWN_ERROR = 4000,
	GATEWAY_CLOSE_UNKNOWN_OPCODE = 4001,
	GATEWAY_CLOSE_DECODE_ERROR = 4002,
	GATEWAY_CLOSE_NOT_AUTHENTICATED = 4003,
	GATEWAY_CLOSE_AUTHENTICATION_FAILED = 4004,
	GATEWAY_CLOSE_ALREADY_AUTHENTICATED = 4005,
	GATEWAY_CLOSE_INVALID_SEQ = 4007,
	GATEWAY_CLOSE_RATE_LIMITED = 4008,
	GATEWAY_CLOSE_SESSION_TIMEOUT = 4009,
	GATEWAY_CLOSE_INVALID_SHARD = 4010,
	GATEWAY_CLOSE_SHARDING_REQUIRED = 4011
};

enum voice_opcode
{
	/* ------ Voice Gateway Opcodes ------ */
	VOICE_GATEWAY_IDENTIFY = 0,
	VOICE_GATEWAY_SELECT_PROTOCOL = 1,
	VOICE_GATEWAY_READY = 2,
	VOICE_GATEWAY_HEARTBEAT = 3,
	VOICE_GATEWAY_SESSION_DESCRIPTION = 4,
	VOICE_GATEWAY_SPEAKING = 5,
	VOICE_GATEWAY_HEARTBEAT_ACK = 6,
	VOICE_GATEWAY_RESUME = 7,
	VOICE_GATEWAY_HELLO = 8,
	VOICE_GATEWAY_RESUMED = 9,
	VOICE_GATEWAY_CLIENT_DISCONNECT = 13,
	/* ------- Voice Close Opcodes ------- */
	VOICE_CLOSE_UNKNOWN_OPCODE = 4001,
	VOICE_CLOSE_NOTH_AUTHENTICATED = 4003,
	VOICE_CLOSE_AUTHENTICATION_FAILED = 4004,
	VOICE_CLOSE_ALREADY_AUTHENTICATED = 4005,
	VOICE_CLOSE_SESSION_INVALID = 4006,
	VOICE_CLOSE_SESSION_TIMEOUT = 4009,
	VOICE_CLOSE_SERVER_NOT_FOUND = 4011,
	VOICE_CLOSE_UNKNOWN_PROTOCOL = 4012,
	VOICE_CLOSE_DISCONNECTED = 4014,
	VOICE_CLOSE_VOICE_SERVER_CRASHED = 4015,
	VOICE_CLOSE_UNKNOWN_ENCRYPTION_MODE = 4016
};

char *get_gateway(const char* query_url);

#endif
