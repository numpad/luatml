#ifndef LUATML_H
#define LUATML_H

#include <lua.h>

#define LUATML_RESULT_TYPE  int
#define LUATML_RESULT_OK    0
#define LUATML_RESULT_ERROR 1

#define LUATML_RETURN_ON_ERROR(expr) do { LUATML_RESULT_TYPE result = (expr); if (result != LUATML_RESULT_OK) return result; } while(0);

typedef struct luatml_ctx_s {
	lua_State *L;
	const char *last_filepath;
} luatml_ctx;

LUATML_RESULT_TYPE luatml_init(luatml_ctx *ctx);
LUATML_RESULT_TYPE luatml_destroy(luatml_ctx *ctx);

LUATML_RESULT_TYPE luatml_loadfile(luatml_ctx *ctx, const char *path);
LUATML_RESULT_TYPE luatml_evalfile(luatml_ctx *ctx);
LUATML_RESULT_TYPE luatml_tohtml(luatml_ctx *ctx, char **output_html);

#endif

