#include "luatml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

LUATML_RESULT_TYPE luatml_init(luatml_ctx *ctx) {
	ctx->L = luaL_newstate();
	luaL_openlibs(ctx->L);
	ctx->last_filepath = NULL;

	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatml_destroy(luatml_ctx *ctx) {
	lua_close(ctx->L);
	ctx->L = NULL;
	ctx->last_filepath = NULL;

	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatml_loadfile(luatml_ctx *ctx, const char *path) {
	if (luaL_loadfile(ctx->L, path) != LUA_OK) {
		const char *error = lua_tostring(ctx->L, -1);
		fprintf(stderr, "could not load file '%s'! error:\n%s\n", path, error);
		lua_pop(ctx->L, 1);
		return LUATML_RESULT_ERROR;
	}

	ctx->last_filepath = path;

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
	lua_getglobal(ctx->L, "html_tostring");
	lua_rotate(ctx->L, -2, 1);
	if (lua_pcall(ctx->L, 1, 1, 0) != LUA_OK) {
		return LUATML_RESULT_ERROR;
	}
	const char *result = lua_tostring(ctx->L, -1);
	
	*output = malloc((strlen(result) + 1) * sizeof(char));
	strncpy(*output, result, strlen(result));

	return LUATML_RESULT_OK;
}

