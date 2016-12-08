#include "server.h"

#ifdef __LINUX__

int tcreate(tfunc_t func, void *arg)
{
	pthread_attr_t 	attr;
	pthread_t	tid;
	
	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, func, arg);
	
	return tid;
}

#else

#endif
