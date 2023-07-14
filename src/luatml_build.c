#include "luatml_build.h"

#include <stdio.h>
#include "luatml.h"
#include "luatml_fs.h"

static LUATML_RESULT_TYPE build_file(luatml_ctx *ctx, const char *path) {
	LUATML_RETURN_ON_ERROR(
		luatml_loadfile(ctx, path)
	);

	LUATML_RETURN_ON_ERROR(
		luatml_evalfile(ctx)
	);

	char *output;
	LUATML_RETURN_ON_ERROR(
		luatml_tohtml(ctx, &output)
	);

	printf("%s\n", output);

	return LUATML_RESULT_OK;
}

static LUATML_RESULT_TYPE build_dir(luatml_ctx *ctx, const char *path) {
	(void)ctx;
	fprintf(stderr, "luatml-build: directories not supported yet. will not build \"%s\"...\n", path);
	return LUATML_RESULT_ERROR;
}

LUATML_RESULT_TYPE luatml_build(luatml_ctx *ctx, int argc, char **argv) {
	if (argc == 0) {
		fprintf(stderr, "luatml-build: missing argument \"path\".\n");
		return LUATML_RESULT_ERROR;
	}

	char *buildtarget_path = argv[0];
	if (luatmlfs_isfile(buildtarget_path)) {
		LUATML_RETURN_ON_ERROR(
			build_file(ctx, argv[0])
		);
	} else if (luatmlfs_isdirectory(buildtarget_path)) {
		LUATML_RETURN_ON_ERROR(
			build_dir(ctx, argv[0])
		);
	}

	return LUATML_RESULT_OK;
}


