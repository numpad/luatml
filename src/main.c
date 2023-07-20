#include <stdio.h>
#include <string.h>
#include "luatml_build.h"
#include "luatml_serve.h"
#include "luatml.h"

static void print_help(int argc, char **argv) {
	(void)argc;

	printf(
		"usage:\n"
		"  %s <command> [options]\n"
		"\n"
		"commands:\n"
		"  build  -  Convert luatml files to static HTML files.\n"
		"  serve  -  Serve luatml files dynamically.\n"
		"\n",
		argv[0]
	);
}

enum LUATML_COMMAND_TYPE {
	LUATML_COMMAND_UNKNOWN,
	LUATML_COMMAND_BUILD, // build input luatml file/directory to html
	LUATML_COMMAND_SERVE, // run a http server to build & serve luatml
	LUATML_COMMAND_PARSE, // parse html to luatml (experimental)
};

static enum LUATML_COMMAND_TYPE parse_luatml_command(char *command) {
	if (command == NULL) {
		return LUATML_COMMAND_UNKNOWN;
	}

	if (strcmp(command, "build") == 0) {
		return LUATML_COMMAND_BUILD;
	} else if (strcmp(command, "serve") == 0) {
		return LUATML_COMMAND_SERVE;
	} else if (strcmp(command, "parse") == 0) {
		return LUATML_COMMAND_PARSE;
	}

	return LUATML_COMMAND_UNKNOWN;
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		print_help(argc, argv);
		return 1;
	}

	luatml_ctx ctx;
	luatml_init_with_args(&ctx, argc - 2, argv + 2);

	char *command_str = argv[1];
	enum LUATML_COMMAND_TYPE command = parse_luatml_command(command_str);
	int exit_code = 1;
	switch (command) {
		case LUATML_COMMAND_BUILD:
			exit_code = luatml_build(&ctx, argc - 2, argv + 2);
			break;
		case LUATML_COMMAND_SERVE:
			exit_code = luatml_serve(&ctx, argc - 2, argv + 2);
			break;
		case LUATML_COMMAND_PARSE:
			fprintf(stderr, "luatml-parse: not implemented\n");
			break;
		case LUATML_COMMAND_UNKNOWN:
			fprintf(stderr, "luatml: unknown command '%s'\n", command_str);
			break;
	}

	luatml_destroy(&ctx);

	return exit_code;
}

