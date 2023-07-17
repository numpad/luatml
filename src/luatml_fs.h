#ifndef LUATML_FS_H
#define LUATML_FS_H

#include "luatml.h"

enum LUATMLFS_ENTRY_TYPE {
	LUATMLFS_UNKNOWN   = 0,
	LUATMLFS_FILE      = 1 << 0,
	LUATMLFS_DIRECTORY = 1 << 1,
};

typedef struct luatmlfs_iterator_s {
	char *path;
	enum LUATMLFS_ENTRY_TYPE mask; // include these entry types

	void *_dir; // meh... crossplatform dies das :)
	struct luatmlfs_iterator_s *sub_iter;
} luatmlfs_iterator;

LUATML_RESULT_TYPE luatmlfs_iterator_init(luatmlfs_iterator *it, const char *basepath);
LUATML_RESULT_TYPE luatmlfs_iterator_destroy(luatmlfs_iterator *it);
char* luatmlfs_next(luatmlfs_iterator *it);

int luatmlfs_isfile(const char *path);
int luatmlfs_isdirectory(const char *path);

LUATML_RESULT_TYPE luatmlfs_mkdir(const char *path);

#endif

