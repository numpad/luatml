#include "luatml_fs.h"

#ifdef _WIN32
#error "WIN32 is not supported yet"

#elif __linux__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int luatmlfs_isfile(const char *path) {
	struct stat attr;
	if (stat(path, &attr) != 0) {
		return 0;
	}
	return S_ISREG(attr.st_mode);
}

int luatmlfs_isdirectory(const char *path) {
	struct stat attr;
	if (stat(path, &attr) != 0) {
		return 0;
	}

	return S_ISDIR(attr.st_mode);
}

#elif __FreeBSD__

#error "FreeBSD is not supported yet"

#elif __APPLE__

#error "APPLE is not supported yet"

#elif __ANDROID__

#error "ANDROID is not supported yet"

#endif

