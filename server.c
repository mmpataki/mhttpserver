
#include "server.h"

int threads_running=0;
int SERVER_PORT=5001;

void *mserver(void *arg)
{
	const int port_no = SERVER_PORT;
	int	  newfd, servfd;
	
	struct work *wrk;
	
	socklen_t clilen;
	struct sockaddr_in s_addr, c_addr;
	
	servfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (servfd < 0) {
		mlog("ERROR opening socket");
	}
	
	bzero((char *) &s_addr, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = INADDR_ANY;
	s_addr.sin_port = htons(port_no);
	
	if(bind(servfd, (struct sockaddr*)&s_addr, sizeof(s_addr)) < 0) {
		mlog("ERROR on binding");
		exit(0);
	}
	
	while(1) {
		listen(servfd, 5);
	while(1) {
	
		clilen = sizeof(c_addr);
		newfd = accept(servfd, (struct sockaddr *) &c_addr, &clilen);

		if (newfd < 0) {
			mlog("ERROR on accept");
			break;
		}
	
		wrk = new_empty_work();		
		wrk->out = newfd;
		wrk->ip = c_addr.sin_addr.s_addr;		
		enque(&rque, wrk);
	}}
	close(servfd);	
}
