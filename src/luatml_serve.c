#include "luatml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <microhttpd.h>
#include "luatml_fs.h"

static LUATML_RESULT_TYPE request_to_file_path(luatml_ctx *ctx, const char *url, char **output) {
	if (ctx->input_path == NULL) {
		fprintf(stderr, "luatml-serve: serving without explicit directory not supported...\n");
		return LUATML_RESULT_ERROR;
	}

	//const char *route_path = ctx->input_path;
	//const size_t route_path_len = strlen(route_path);
	const size_t route_path_len = strlen(ctx->input_path) + 1 + strlen(ctx->path_to_routes) + 1;
	char route_path[route_path_len];
	sprintf(route_path, "%s/%s", ctx->input_path, ctx->path_to_routes);
	
	// special handling for "/"
	if (strcmp(url, "/") == 0) {
		const char *index = "index.lua";
		*output = malloc(route_path_len + 1 + strlen(index) + 1);
		sprintf(*output, "%s/%s", route_path, index);
		return LUATML_RESULT_OK;
	}

	// urls that map to a .lua file
	size_t url_len = strlen(url);
	while (url[url_len - 1] == '/' && url_len > 0) {
		url_len -= 1;
	}

	char clean_url[url_len + 1];
	strncpy(clean_url, url, url_len);
	clean_url[url_len] = '\0';

	// /about/ → /about.lua
	const size_t direct_file_len = route_path_len + 1 + url_len + strlen(".lua") + 1;
	char direct_file[direct_file_len];
	sprintf(direct_file, "%s/%s.lua", route_path, clean_url);

	// /about/ → /about/index.lua
	// const index_file_len = route_path_len + 1 + url_len + 1 + strlen("index.lua") + 1;
	// char index_file[index_file_len];
	// sprintf(index_file, "%s/%s/index.lua", route_path, clean_url, "index.lua");
	
	*output = malloc(strlen(direct_file) + 1);
	strcpy(*output, direct_file);

	return LUATML_RESULT_OK;
}

static enum MHD_Result on_request(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **conn_cls) {
	struct sockaddr_in **addr_in = (struct sockaddr_in **)MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
	printf("%15s  %s  %-7s  %s\n", inet_ntoa((*addr_in)->sin_addr), version, method, url);
	
	// get userdata
	luatml_ctx *ctx = (luatml_ctx *)cls;

	char *request_path = NULL;
	if (request_to_file_path(ctx, url, &request_path) != LUATML_RESULT_OK) {
		fprintf(stderr, "could not convert url to file path!\n");
		return MHD_NO; // TODO: not a fix
	}

	int result = MHD_NO;
	if (luatmlfs_isfile(request_path)) {
		char *html;
		luatml_convertfile(ctx, request_path, &html);
		struct MHD_Response *response = MHD_create_response_from_buffer(strlen(html), html, MHD_RESPMEM_PERSISTENT);
		result = MHD_queue_response(connection, 200, response);
		MHD_destroy_response(response);
	} else {
		const char *body = "<!DOCTYPE html><html><head><title>file not found</title></head><body><h1>404 - file not found</h1><p>the requested url does not exist.</p></body></html>";
		struct MHD_Response *response = MHD_create_response_from_buffer(strlen(body), (void*)body, MHD_RESPMEM_PERSISTENT);
		result = MHD_queue_response(connection, 404, response);
		MHD_destroy_response(response);
	}

	if (request_path != NULL) {
		free(request_path);
	}

	return result;
}

LUATML_RESULT_TYPE luatml_serve(luatml_ctx *ctx, int argc, char **argv) {
	if (argc == 0) {
		fprintf(stderr, "luatml-serve: no input given!\n");
		return LUATML_RESULT_ERROR;
	}

	const char *input = ctx->input_path;
	if (!luatmlfs_isdirectory(input)) {
		fprintf(stderr, "luatml-serve: server requires an input directory...\n");
	}

	const int port = 8084;
	ctx->server = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, port, NULL, NULL, &on_request, ctx, MHD_OPTION_END);

	if (ctx->server == NULL) {
		fprintf(stderr, "luatml-server: could not start server on port :%d...\n", port);
		return LUATML_RESULT_ERROR;
	}

	printf("luatml-serve: running server on :%d, press enter to exit\n", port);
	getchar();

	return LUATML_RESULT_OK;
}

