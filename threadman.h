#ifdef __LINUX__

#include<pthread.h>

typedef void *(*tfunc_t)(void*);

int tcreate(tfunc_t, void *);


#else


#endif
