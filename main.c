#include "server.h"
#include<sys/signal.h>

/* Request Queue shared among two threads */
struct g_queue rque;
struct system_t system;

int main(int argc, char *argv[])
{
	signal(SIGPIPE, SIG_IGN);
	if(argc > 1) {
		SERVER_PORT = atoi(argv[1]);
	}

	//scan arguments.
	for(i=2; i<argc; i++) {
		if(!strcmp(argv[i], "pout")) {
			system.pprint = 1;
			system.pout = stdout;
		}
	}


	/* create thread for processing the requests */
	tcreate(mserver, NULL);

	/* begin processing requests. */
	struct work *wrk;

	while(1) {
		wrk = get_work();
		dispatch_work(wrk);
	}
}

struct work *get_work()
{
	struct work *wrk=NULL;
	
	while(wrk == NULL)
		wrk = (struct work*)deque(&rque);

	return wrk;
}

void dispatch_work(struct work *wrk)
{	
	if(threads_running < MAX_THREADS) {
		threads_running++;
		tcreate((tfunc_t)serve_user, wrk);
	}
}
