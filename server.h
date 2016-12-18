

/* define this if compiling for LINUX */
#define __LINUX__ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "queue.h"
#include "threadman.h"
#include "work.h"

#define MAX_THREADS 10
#define mlog printf
#define HTTP_VERSION 1.1

struct system_t {
	int pprint;
	FILE *pout;
};

extern int threads_running;
extern int SERVER_PORT;
extern struct system_t system;

/* Request Queue shared among two threads */
extern struct g_queue rque;

struct work *get_work();
void dispatch_work(struct work *wrk);
void *mserver(void *arg);
