#include "luatml_lib.h"

#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include "luatml.h"
#include "luatml_fs.h"

static int luatml_lib_listfiles(lua_State *L) {
	const char *arg_path = lua_tostring(L, 1);
	if (arg_path == NULL) {
		luaL_error(L, "Function takes 1 argument, 0 given.\n");
		return 0;
	}
	
	lua_newtable(L);

	luatmlfs_iterator it;
	luatmlfs_iterator_init(&it, arg_path);
	it.mask = LUATMLFS_FILE;

	int table_index = 1;
	char *currentpath;
	while ((currentpath = luatmlfs_next(&it)) != NULL) {
		lua_pushstring(L, currentpath);
		lua_rawseti(L, -2, table_index++);
	}

	return 1;
}

LUATML_RESULT_TYPE luatml_lib_openlibs(luatml_ctx *ctx) {
	assert(ctx->L != NULL);

	lua_register(ctx->L, "luatml_listfiles", luatml_lib_listfiles);
}

