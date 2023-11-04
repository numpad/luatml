#ifndef LUATML_H
#define LUATML_H

#include <lua.h>
#include <microhttpd.h>

#define LUATML_RESULT_TYPE  int
#define LUATML_RESULT_OK    0
#define LUATML_RESULT_ERROR 1

#define LUATML_RETURN_ON_ERROR(expr) do { const LUATML_RESULT_TYPE __result = (expr); if (__result != LUATML_RESULT_OK) return __result; } while(0);

typedef struct luatml_ctx_s {
	lua_State *L;

	char *input_path;     // path to the luatml project.
	char *path_to_routes; // (optional) path to the "routes" folder in the project.
	char *output_path;    // (optional) output path when `build`ing.

	// only set for luatml-serve
	struct MHD_Daemon *server;
} luatml_ctx;

// init & destroy
LUATML_RESULT_TYPE luatml_init(luatml_ctx *ctx);
LUATML_RESULT_TYPE luatml_init_with_args(luatml_ctx *ctx, int argc, char **argv);
LUATML_RESULT_TYPE luatml_destroy(luatml_ctx *ctx);

LUATML_RESULT_TYPE luatml_reload_lua(luatml_ctx *ctx);

LUATML_RESULT_TYPE luatml_loadfile(luatml_ctx *ctx, const char *path);
LUATML_RESULT_TYPE luatml_evalfile(luatml_ctx *ctx);
LUATML_RESULT_TYPE luatml_tohtml(luatml_ctx *ctx, char **output_html);
// shortcut for the 3 functions above
LUATML_RESULT_TYPE luatml_convertfile(luatml_ctx *ctx, const char *path, char **output);

#endif

