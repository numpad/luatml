#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <arpa/inet.h>

static lua_State *L;

int luatml_loadfile(lua_State *L, const char *path) {
	{ // debug only
		static const char *last_path = NULL;
		if (path != NULL) {
			last_path = path;
		}

		if (path == NULL) {
			path = last_path;
		}
		printf("path = %s, last = %s\n", path, last_path);

		// TODO: also set our "libs"/"components" in `package.loaded` to nil so we trigger a reload in debug
	}

	if (luaL_loadfile(L, path) != LUA_OK) {
		const char *error = lua_tostring(L, -1);
		fprintf(stderr, "could not load file '%s'! error:\n%s\n", path, error);
		lua_pop(L, 1);
		return 1;
	}

	return 0;
}

int luatml_eval(lua_State *L) {
	if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
		const char *error = lua_tostring(L, -1);
		fprintf(stderr, "failed executing script.\n%s\n", error);
		lua_pop(L, 1);

		return 1;
	}

	return 0;
}

int luatml_tostring(lua_State *L, const char **output) {
	if (!lua_istable(L, -1)) {
		return 1;
	}
	
	// did we return a `html {...}` tag, or just a table?
	lua_getfield(L, -1, "tag");
	const int is_htmltag = lua_isstring(L, -1);
	lua_pop(L, 1);
	if (!is_htmltag) {
		return 1;
	}

	// convert to string
	lua_getglobal(L, "html_tostring");
	lua_rotate(L, -2, 1);
	if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
		return 1;
	}
	const char *result = lua_tostring(L, -1);
	
	*output = malloc((strlen(result) + 1) * sizeof(char));
	strncpy(*output, result, strlen(result));

	return 0;
}

static enum MHD_Result on_request(void *cls, struct MHD_Connection *connection,
		const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {

	luatml_loadfile(L, NULL);

	// debug log
	struct sockaddr_in **addr_in = (struct sockaddr_in **)MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
	printf("%15s  %s  %-7s  %s\n", inet_ntoa((*addr_in)->sin_addr), version, method, url);

	// prepare script
	lua_pushvalue(L, -1);
	// prepare request object
	lua_newtable(L);
	lua_pushstring(L, url);
	lua_setfield(L, -2, "url");
	lua_pushstring(L, method);
	lua_setfield(L, -2, "method");
	lua_pushstring(L, version);
	lua_setfield(L, -2, "version");
	lua_setglobal(L, "request");

	if (luatml_eval(L)) {
		// return HTTP 500
	}

	// create response body
	int res_status = 200;
	const char *res_location = NULL;
	const char *res_body = NULL;
	if (lua_isstring(L, -1)) {
		res_body = lua_tostring(L, -1);
	} else if (lua_istable(L, -1)) {
		// did we return a `html {...}` tag, or just a table?
		lua_getfield(L, -1, "tag");
		const int is_htmltag = lua_isstring(L, -1);
		lua_pop(L, 1);

		if (is_htmltag) {
			luatml_tostring(L, &res_body);
			res_body = lua_tostring(L, -1);
		} else {
			// normal response table(?)
			lua_getfield(L, -1, "status");
			if (lua_isnumber(L, -1)) {
				res_status = lua_tointeger(L, -1);
			}
			lua_pop(L, 1);

			lua_getfield(L, -1, "location");
			if (lua_isstring(L, -1)) {
				res_location = lua_tostring(L, -1);
			}
			lua_pop(L, 1);

			// TODO: cookies, content type, ...
			lua_getfield(L, -1, "body");
			if (lua_isstring(L, -1)) {
				res_body = lua_tostring(L, -1);
			} else if (lua_istable(L, -1)) {
				// handle html_tag
				// convert to string
				lua_getglobal(L, "html_tostring");
				lua_rotate(L, -2, 1);
				if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
					// error
				}
				res_body = lua_tostring(L, -1);
			}
			lua_pop(L, 1);
		}
	}

	if (res_status >= 300 && res_status <= 399 && res_location != NULL) {
		struct MHD_Response *response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
		MHD_add_response_header (response, "Location", res_location);
		const int ret = MHD_queue_response(connection, res_status, response);
		MHD_destroy_response(response);
		lua_pop(L, 1);
		return ret;
	}

	// could not convert page
	if (res_body == NULL) {
		fprintf(stderr, "not a valid ");
		// return early, error
	}

	struct MHD_Response *response = MHD_create_response_from_buffer(strlen(res_body), (void*)res_body, MHD_RESPMEM_PERSISTENT);
	const int ret = MHD_queue_response(connection, res_status, response);
	MHD_destroy_response(response);
	lua_pop(L, 1);
	return ret;
}

int main(int argc, char** argv) {
	if (argc <= 1) {
		printf(
			"usage: %s <script>\n"
			"       -b, --bind   bind to specific address.\n"
			"       -p, --port   use specified port.\n"
			"       -d, --debug  run in debug mode.\n"
			, argv[0]);
		return 1;
	}

	// start lua
	L = luaL_newstate();
	luaL_openlibs(L);
	
	if (luatml_loadfile(L, argv[1])) {
		return 1;
	}

	// start server
	const int port = 8004;
	struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
			port, NULL, NULL, &on_request, NULL, MHD_OPTION_END);
	
	printf("starting server on :%d...\n", port);
	getc(stdin);
	printf("stopping server...\n");

	lua_close(L);
	MHD_stop_daemon(daemon);
	return 0;
}

