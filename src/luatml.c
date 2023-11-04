#include "luatml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "luatml_fs.h"

static LUATML_RESULT_TYPE init_lua_state(lua_State **L) {
	assert(L != NULL);
	assert(*L == NULL);

	*L = luaL_newstate();
	if (*L == NULL) {
		fprintf(stderr, "luatml: failed to initialize lua state.\n");
		return LUATML_RESULT_ERROR;
	}

	luaL_openlibs(*L);
	return LUATML_RESULT_OK;
}

static LUATML_RESULT_TYPE add_package_search_dir(luatml_ctx *ctx, const char *path_to_dir) {
	if (!luatmlfs_isdirectory(path_to_dir)) {
		return LUATML_RESULT_ERROR;
	}

	// build the string to append to "package.path"
	const char *path_addition_suffix = "/?.lua";
	const size_t path_addition_len = 1 + strlen(path_to_dir) + strlen(path_addition_suffix) + 1;
	char path_addition[path_addition_len];
	sprintf(path_addition, ";%s%s", path_to_dir, path_addition_suffix);

	// overwrite existing package path
	lua_getglobal(ctx->L, "package");
	lua_getfield(ctx->L, -1, "path");
	const char *prev_path = lua_tostring(ctx->L, -1);
	const size_t prev_path_len = strlen(prev_path);
	const size_t new_path_len = prev_path_len + path_addition_len;
	char newpath[new_path_len];
	sprintf(newpath, "%s%s", prev_path, path_addition);
	lua_pop(ctx->L, 1);
	lua_pushstring(ctx->L, newpath);
	lua_setfield(ctx->L, -2, "path");
	lua_pop(ctx->L, 1);

	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatml_init(luatml_ctx *ctx) {
	assert(ctx != NULL);

	ctx->input_path = NULL;
	ctx->path_to_routes = ".";
	ctx->output_path = NULL;
	ctx->server = NULL;
	ctx->L = NULL;

	const int result = init_lua_state(&ctx->L);
	LUATML_RETURN_ON_ERROR(result);
	
	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatml_init_with_args(luatml_ctx *ctx, int argc, char **argv) {
	LUATML_RETURN_ON_ERROR(luatml_init(ctx));
	
	const char *option_waiting_for_arg = NULL;
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "--") == 0) break;

		const int is_flag = (argv[i][0] == '-');
		const int is_flag_value = !is_flag && (option_waiting_for_arg != NULL);
		const int is_positional = !is_flag && !is_flag_value;

		if (is_positional && ctx->input_path == NULL) {
			ctx->input_path = argv[i];
		} else if (is_flag) {
			if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
				option_waiting_for_arg = "--output";
			} else if (strcmp(argv[i], "-R") == 0 || strcmp(argv[i], "--routes") == 0) {
				option_waiting_for_arg = "--routes";
			}
		} else if (is_flag_value) {
			if (strcmp(option_waiting_for_arg, "--output") == 0) {
				ctx->output_path = argv[i];
			} else if (strcmp(option_waiting_for_arg, "--routes") == 0) {
				ctx->path_to_routes = argv[i];
			}
			option_waiting_for_arg = NULL;
		}
	}

	const int result = add_package_search_dir(ctx, ctx->input_path);
	LUATML_RETURN_ON_ERROR(result);

	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatml_destroy(luatml_ctx *ctx) {
	if (ctx == NULL) {
		return LUATML_RESULT_ERROR;
	}

	lua_close(ctx->L);
	ctx->L = NULL;
	MHD_stop_daemon(ctx->server);

	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatml_reload_lua(luatml_ctx *ctx) {
	assert(ctx != NULL);
	
	lua_close(ctx->L);
	ctx->L = NULL;

	int result;
	result = init_lua_state(&ctx->L);
	LUATML_RETURN_ON_ERROR(result);

	result = add_package_search_dir(ctx, ctx->input_path);
	LUATML_RETURN_ON_ERROR(result);

	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatml_loadfile(luatml_ctx *ctx, const char *path) {
	if (luaL_loadfile(ctx->L, path) != LUA_OK) {
		const char *error = lua_tostring(ctx->L, -1);
		fprintf(stderr, "could not load file '%s'! error:\n%s\n", path, error);
		lua_pop(ctx->L, 1);
		return LUATML_RESULT_ERROR;
	}

	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatml_evalfile(luatml_ctx *ctx) {
	if (lua_pcall(ctx->L, 0, 1, 0) != LUA_OK) {
		const char *error = lua_tostring(ctx->L, -1);
		fprintf(stderr, "failed executing script.\n%s\n", error);
		lua_pop(ctx->L, 1);

		return LUATML_RESULT_ERROR;
	}
	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatml_tohtml(luatml_ctx *ctx, char **output) {
	if (!lua_istable(ctx->L, -1)) {
		return LUATML_RESULT_ERROR;
	}
	
	// did we return a `html {...}` tag, or just a table?
	lua_getfield(ctx->L, -1, "tag");
	const int is_htmltag = lua_isstring(ctx->L, -1);
	lua_pop(ctx->L, 1);
	if (!is_htmltag) {
		return LUATML_RESULT_ERROR;
	}

	// convert to string
	lua_getglobal(ctx->L, "html_tostring"); // TODO: implement this in c
	//lua_rotate(ctx->L, -2, 1); // only lua5.3+, we are targeting 5.1...
	lua_replace(ctx->L, -3); // TODO: not really tested

	if (lua_pcall(ctx->L, 1, 1, 0) != LUA_OK) {
		return LUATML_RESULT_ERROR;
	}
	const char *result = lua_tostring(ctx->L, -1);
	
	const int result_len = strlen(result);
	*output = malloc((result_len + 1) * sizeof(char));
	(*output)[result_len] = '\0';
	strncpy(*output, result, strlen(result));

	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatml_convertfile(luatml_ctx *ctx, const char *path, char **output) {
	LUATML_RETURN_ON_ERROR(
		luatml_loadfile(ctx, path)
	);

	LUATML_RETURN_ON_ERROR(
		luatml_evalfile(ctx)
	);

	LUATML_RETURN_ON_ERROR(
		luatml_tohtml(ctx, output)
	);

	return LUATML_RESULT_OK;
}

