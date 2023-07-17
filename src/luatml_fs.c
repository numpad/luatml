#include "luatml_fs.h"

#ifdef __linux__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

static int luatmlfs_entrytype(const char *path) {
	struct stat attr;
	if (stat(path, &attr) != 0) {
		return 0;
	}

	if (S_ISREG(attr.st_mode)) {
		return LUATMLFS_FILE;
	} else if (S_ISDIR(attr.st_mode)) {
		return LUATMLFS_DIRECTORY;
	}

	return LUATMLFS_UNKNOWN;
}

int luatmlfs_isfile(const char *path) {
	return luatmlfs_entrytype(path) == LUATMLFS_FILE;
}

int luatmlfs_isdirectory(const char *path) {
	return luatmlfs_entrytype(path) == LUATMLFS_DIRECTORY;
}

LUATML_RESULT_TYPE luatmlfs_iterator_init(luatmlfs_iterator *it, const char *basepath) {
	if (!luatmlfs_isdirectory(basepath)) {
		return LUATML_RESULT_ERROR;
	}

	it->path = malloc(strlen(basepath) + 1);
	it->mask = LUATMLFS_FILE | LUATMLFS_DIRECTORY;
	strcpy(it->path, basepath);
	it->_dir = opendir(basepath);
	it->sub_iter = NULL;

	return LUATML_RESULT_OK;
}

LUATML_RESULT_TYPE luatmlfs_iterator_destroy(luatmlfs_iterator *it) {
	free(it->path);
	closedir(it->_dir);
	it->_dir = NULL;
	if (it->sub_iter != NULL) {
		luatmlfs_iterator_destroy(it->sub_iter);
	}
	it->sub_iter = NULL;

	return LUATML_RESULT_OK;
}

char* luatmlfs_next(luatmlfs_iterator *it) {
	luatmlfs_iterator *iterator = it;
	luatmlfs_iterator *parent = NULL;
	while (iterator->sub_iter != NULL) {
		parent = iterator;
		iterator = iterator->sub_iter;
	}
	
	struct dirent *entry;
	while ((entry = readdir(iterator->_dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		// combine path & allocate memory
		const int fullpath_len = strlen(iterator->path) + strlen(entry->d_name) + 2; // 2 because \0 and the extra slash at "%s/%s"
		char *fullpath = malloc(fullpath_len);
		snprintf(fullpath, fullpath_len, "%s/%s", iterator->path, entry->d_name);
		
		enum LUATMLFS_ENTRY_TYPE ftype = luatmlfs_entrytype(fullpath);
		switch (ftype) {
		case LUATMLFS_FILE:
			if (it->mask & LUATMLFS_FILE) {
				return fullpath;
			} else {
				free(fullpath);
				continue;
			}
		case LUATMLFS_DIRECTORY: {
			luatmlfs_iterator *sub_iter = malloc(sizeof(luatmlfs_iterator));
			luatmlfs_iterator_init(sub_iter, fullpath);
			sub_iter->mask = it->mask; // TODO: needed? maybe add luatmlfs_iterator_copy() or similar
			iterator->sub_iter = sub_iter;
			if (it->mask & LUATMLFS_DIRECTORY) {
				return fullpath;
			} else {
				return luatmlfs_next(sub_iter);
			}
		}
		case LUATMLFS_UNKNOWN:
			fprintf(stderr, "encountered unkown file type at: %s\n", fullpath);
			continue;
		};

	}
	
	if (parent != NULL) {
		luatmlfs_iterator_destroy(parent->sub_iter);
		parent->sub_iter = NULL;
		return luatmlfs_next(it);
	}

	return NULL;
}

LUATML_RESULT_TYPE luatmlfs_mkdir(const char *path) {
	if (mkdir(path, 0755) != 0) {
		return LUATML_RESULT_ERROR;
	}
	return LUATML_RESULT_OK;
}

#elif _WIN32
#error "WIN32 is not supported yet"

#elif __FreeBSD__

#error "FreeBSD is not supported yet"

#elif __APPLE__

#error "APPLE is not supported yet"

#elif __ANDROID__

#error "ANDROID is not supported yet"

#endif

