#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static lua_State *L;

static enum MHD_Result on_request(void *cls, struct MHD_Connection *connection,
		const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **ptr) {

	// run script, 
	lua_pushvalue(L, -1);
	if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
		const char *error = lua_tostring(L, -1);
		fprintf(stderr, "failed executing script.\n%s\n", error);
		lua_pop(L, 1);

		// return early with http 500 and error message
	}

	const char *page = luaL_checkstring(L, -1);
	if (page == NULL) {
		fprintf(stderr, "return value must be of type string!\n");

		// return error
	}

	struct MHD_Response *response = MHD_create_response_from_buffer(
			strlen(page), (void*)page, MHD_RESPMEM_PERSISTENT);
	
	const int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	lua_pop(L, 1);
	return ret;
}

int main(int argc, char** argv) {
	if (argc <= 1) {
		printf(
			"usage: %s <script>\n"
			"       -b, --bind  bind to specific address.\n"
			"       -p, --port  use specified port.\n"
			, argv[0]);
		return 1;
	}

	// start lua
	L = luaL_newstate();
	luaL_openlibs(L);

	if (luaL_loadfile(L, argv[1]) != LUA_OK) {
		const char *error = lua_tostring(L, -1);
		fprintf(stderr, "could not load file '%s'! error:\n%s\n", argv[1], error);
		lua_pop(L, 1);
		return 1;
	}

	// start server
	const int port = 8004;
	struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
			port, NULL, NULL, &on_request, NULL, MHD_OPTION_END);
	
	getc(stdin);

	lua_close(L);
	MHD_stop_daemon(daemon);
	return 0;
}

