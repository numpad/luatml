#include "luatml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "luatml_fs.h"

LUATML_RESULT_TYPE luatml_init(luatml_ctx *ctx) {
	ctx->L = luaL_newstate();
	if (ctx->L == NULL) {
		fprintf(stderr, "luatml: failed to initialize lua state.\n");
		return LUATML_RESULT_ERROR;
	}
	luaL_openlibs(ctx->L);

	ctx->input_path = NULL;
	ctx->output_path = NULL;
	ctx->server = NULL;


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
			}
		} else if (is_flag_value) {
			if (strcmp(option_waiting_for_arg, "--output") == 0) {
				ctx->output_path = argv[i];
			}
			option_waiting_for_arg = NULL;
		}
	}

	if (luatmlfs_isdirectory(ctx->input_path)) {
		// build the string to append to "package.path"
		const char *path_addition_suffix = "/?.lua";
		const size_t path_addition_len = 1 + strlen(ctx->input_path) + strlen(path_addition_suffix) + 1;
		char path_addition[path_addition_len];
		sprintf(path_addition, ";%s%s", ctx->input_path, path_addition_suffix);

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
	}

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

