#ifndef LUATML_H
#define LUATML_H

#include <lua.h>
#include <microhttpd.h>

#define LUATML_RESULT_TYPE  int
#define LUATML_RESULT_OK    0
#define LUATML_RESULT_ERROR 1

#define LUATML_RETURN_ON_ERROR(expr) do { const LUATML_RESULT_TYPE result = (expr); if (result != LUATML_RESULT_OK) return result; } while(0);

typedef struct luatml_ctx_s {
	lua_State *L;

	char *input_path;
	char *output_path;

	// only set for luatml-serve
	struct MHD_Daemon *server;
} luatml_ctx;

LUATML_RESULT_TYPE luatml_init(luatml_ctx *ctx);
LUATML_RESULT_TYPE luatml_init_with_args(luatml_ctx *ctx, int argc, char **argv);
LUATML_RESULT_TYPE luatml_destroy(luatml_ctx *ctx);

LUATML_RESULT_TYPE luatml_loadfile(luatml_ctx *ctx, const char *path);
LUATML_RESULT_TYPE luatml_evalfile(luatml_ctx *ctx);
LUATML_RESULT_TYPE luatml_tohtml(luatml_ctx *ctx, char **output_html);
// shortcut for the 3 functions above
LUATML_RESULT_TYPE luatml_convertfile(luatml_ctx *ctx, const char *path, char **output);

#endif

