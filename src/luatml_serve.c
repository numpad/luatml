#include "luatml.h"

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <microhttpd.h>
#include "luatml_fs.h"


static enum MHD_Result on_request(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **conn_cls) {
	struct sockaddr_in **addr_in = (struct sockaddr_in **)MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
	printf("%15s  %s  %-7s  %s\n", inet_ntoa((*addr_in)->sin_addr), version, method, url);
	
	// get userdata
	luatml_ctx *ctx = (luatml_ctx *)cls;
	char request_path[strlen(ctx->input_path) + strlen(url) + 1];
	sprintf(request_path, "%s%s", ctx->input_path, url);

	printf("requested: %s\n  input is: %s\n", request_path, ctx->input_path);

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

	return result;
}

LUATML_RESULT_TYPE luatml_serve(luatml_ctx *ctx, int argc, char **argv) {
	if (argc == 0) {
		fprintf(stderr, "luatml-serve: no input given!\n");
		return LUATML_RESULT_ERROR;
	}

	const char *input = argv[0];
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

