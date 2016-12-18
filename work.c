#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "httph.h"

struct g_queue users;

struct work *new_empty_work()
{
	struct work *nw = malloc(sizeof(struct work));
	if (!nw)
	{
		printf("malloc failed on work creation");
		exit(0);
	}
	return nw;
}

void sendm(int out, void *kvp, int sz)
{
	write(out, kvp, sz);
}

const char *get_action(int actioncode)
{
	switch(actioncode)
	{
	case POST:
		return "POST";
	case GET:
		return "GET";
	case HEAD:
		return "HEAD";
	case PUT:
		return "PUT";
	}
}

void send_http_header(int out, struct http_response *response)
{
	char buffer[1000], *cptr, *status_string;
	cptr = buffer;

	switch(response->status)
	{
	case 200:
		status_string = "OK.";
		break;
	case 403:
		status_string = "Access Forbidden.";
		break;
	case 404:
		status_string = "Not Found.";
		break;
	}

	sendm(out, buffer, 
		sprintf(buffer,
			"HTTP/%2.1f, %d %s\r\nConnection: Close\r\n\r\n",
			HTTP_VERSION,
			response->status,
			status_string 
		)
	);

}

char *build_response(struct http_request *request,
	struct http_response *response)
{
	char *sending_file;
	FILE *fptr = NULL;

	response->status = 200;
	sending_file = request->url;

	if(sending_file[0] == '\0')
	{
		sending_file = "index.html";
	}

	if (strstr(sending_file, ".."))
	{
		response->status = 403;
		sending_file = "hackers.html";
	}

	if (!(fptr = fopen(sending_file, "r")))
	{
		response->status = 404;
		sending_file = "404.html";

		if(!(fptr = fopen(sending_file, "r"))) {
			printf("-100 internal error [%s not found]\n", sending_file);
			sending_file = NULL;
		}
	}

	if(fptr)	
		fclose(fptr);
	return sending_file;
}

void send_file(int out, struct http_request *request)
{
	FILE *fptr;
	int x, total = 0;
	char buffer[1000], *temp_str;
	struct http_response response;

	if((temp_str = build_response(request, &response)) == NULL)
		return;

	fptr = fopen(temp_str, "r");

	send_http_header(out, &response);

	do {
		while (x < 1000 && !feof(fptr))
			buffer[x++] = getc(fptr);

		if(feof(fptr))
			buffer[--x] = 0;
		sendm(out, buffer, x);
		
		total += x;
		x = 0;
	} while(!feof(fptr));

	fclose(fptr);
	
	fprintf(stderr, " <- %5s %d %9d bytes /%s\n", get_action(request->action), response.status, total, request->url);
	fprintf(stdout, " <- %5s %d %9d bytes /%s\n", get_action(request->action), response.status, total, request->url);
}

void handle_post(int out, struct http_request *req)
{
	pid_t pid = fork();

	char str[100] = "./", data[1024];
	strcat(str, req->url);
	sprintf(data, "%s", req->data);


	if (pid == 0)
	{
		dup2(out, 1);
		execlp(str, str, data, NULL);
	}

	fprintf(stderr, " <- %5s /%s\n", get_action(req->action), req->url);
	fprintf(stdout, " <- %5s /%s\n", get_action(req->action), req->url);
}

void serve_user(struct work *wrk)
{
	int len;
	struct http_request request;
	struct http_response response;

accept_new_request:
	/* read the request from client */
	if (len = read(wrk->out, wrk, COMMAND_MAX_LEN) < 5)
	{
		wrk->data[len] = 0;
		goto shut_out;
	}

	if(system.ppacket) {
		fwrite(wrk, len, system.pout);
	}

#define WORDN(x, size, n) ((x & (0xFF << n)) >> (size - (n * 8)))
#define IP4BYTES(nip) WORDN(nip, 31, 0),WORDN(nip, 31, 1),WORDN(nip, 31, 2),WORDN(nip, 31, 3)

	/* print ip address */
	fprintf(stderr, "%d.%d.%d.%d", IP4BYTES(wrk->ip));
	fprintf(stdout, "%d.%d.%d.%d", IP4BYTES(wrk->ip));
	
	parse_httpheader((char *)wrk, &request);
	
	switch(request.action)
	{
	case HEAD:
		build_response(&request, &response);
		send_http_header(wrk->out, &response);
		break;
	case GET:
		if(strstr(request.url, ".masp")) {
			build_response(&request, &response);
			send_http_header(wrk->out, &response);
			handle_post(wrk->out, &request);
		}
		else
			send_file(wrk->out, &request);
		break;
	case POST:
		handle_post(wrk->out, &request);
		break;
	case PUT:
	default:
		break;
	}

	switch(request.connection)
	{
	case CLOSE:
		close(wrk->out);
		break;
	case KEEP_ALIVE:
		close(wrk->out);
		break;
	}

shut_out:
	threads_running--;
	pthread_exit(0);
}
