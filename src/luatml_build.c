#include "luatml_build.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include "luatml.h"
#include "luatml_fs.h"

static LUATML_RESULT_TYPE build_file(luatml_ctx *ctx, const char *path, const char *output_path) {
	char *output;
	LUATML_RETURN_ON_ERROR(
		luatml_convertfile(ctx, path, &output)
	);

	FILE *fd = stdout;
	if (output_path != NULL) {
		fd = fopen(output_path, "w+");
		if (fd == NULL) {
			fprintf(stderr, "luatml-build: not a valid output path\n");
			return LUATML_RESULT_ERROR;
		}
	}

	fprintf(fd, "%s\n", output);

	if (output_path != NULL) {
		fclose(fd);
	}

	return LUATML_RESULT_OK;
}

static LUATML_RESULT_TYPE build_dir(luatml_ctx *ctx, const char *path, const char *output_path) {
	if (output_path == NULL || !luatmlfs_isdirectory(output_path)) {
		fprintf(stderr, "luatml-build: not a valid output path\n");
		return LUATML_RESULT_ERROR;
	}

	// create directory structure first
	{
		luatmlfs_iterator it;
		luatmlfs_iterator_init(&it, path);
		it.mask = LUATMLFS_DIRECTORY;

		char *currentpath;
		while ((currentpath = luatmlfs_next(&it)) != NULL) {
			char dirpath[strlen(output_path) + strlen(currentpath + strlen(path) + 2)];
			sprintf(dirpath, "%s/%s", output_path, currentpath + strlen(path) + 1);
			free(currentpath);
			
			if (luatmlfs_isdirectory(dirpath)) {
				fprintf(stderr, "luatml-build: directory \"%s\" already exists.\n", dirpath);
				continue;
			}

			if (luatmlfs_mkdir(dirpath) == LUATML_RESULT_ERROR) {
				fprintf(stderr, "luatml-build: failed to create dir \"%s\"...\n", dirpath);
			}

		}

		luatmlfs_iterator_destroy(&it);
	}

	// generate files now
	{
		luatmlfs_iterator it;
		luatmlfs_iterator_init(&it, path);
		it.mask = LUATMLFS_FILE;

		char *currentpath;
		while ((currentpath = luatmlfs_next(&it)) != NULL) {
			const char *extension = strrchr(currentpath, '.');
			const char *newextension = extension;
			if (strcmp(extension, ".lua") == 0) {
				newextension = ".html";
			}

			const int dotext_len = strlen(extension);
			const int dothtml_len = strlen(newextension);
			const char *relativepath = currentpath + strlen(path);
			// TODO: rework this...
			char filepath[strlen(output_path) + 1 + strlen(relativepath + 1 - dotext_len + dothtml_len)];
			filepath[0] = '\0';
			sprintf(filepath, "%s/", output_path);
			snprintf(filepath + strlen(filepath), strlen(relativepath + 0) - dotext_len, "%s", relativepath + 1);
			strcat(filepath, newextension);

			fprintf(stderr, "%s -> %s\n", relativepath + 1, filepath); // instead of relativepath we could to currentpath

			if (strcmp(extension, ".lua") == 0) {
				build_file(ctx, currentpath, filepath);
			} else {
				luatmlfs_copyfile(currentpath, filepath);
			}

			free(currentpath);
		}

		luatmlfs_iterator_destroy(&it);
	}
	

	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatml_build(luatml_ctx *ctx, int argc, char **argv) {
	if (ctx->input_path == NULL) {
		fprintf(stderr, "luatml-build: no input path given...\n");
		return LUATML_RESULT_ERROR;
	}

	if (luatmlfs_isfile(ctx->input_path)) {
		LUATML_RETURN_ON_ERROR(
			build_file(ctx, ctx->input_path, ctx->output_path)
		);
	} else if (luatmlfs_isdirectory(ctx->input_path)) {
		LUATML_RETURN_ON_ERROR(
			build_dir(ctx, ctx->input_path, ctx->output_path)
		);
	}

	return LUATML_RESULT_OK;
}


