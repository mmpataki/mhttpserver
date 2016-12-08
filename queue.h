
/*
 * Not an ADT. Built specifically for this application
 * Using lock as the two threads can end-up with deadlock.
 */

#include<stdlib.h>

struct queue_ent
{
    void             *data;
    struct queue_ent *next;
};

struct g_queue
{
    /* to prevent deadlocks */
    int     lock;

    /* count of requests */
    int     cnt;

    /* ends of queue */
    struct queue_ent *first, *last;
};


void wait_n_lock(struct g_queue *q);
void unlock(struct g_queue *q);
struct queue_ent *new_queue_ent(void *data);
void enque(struct g_queue *q, void *data);
void *deque(struct g_queue *q);
int isempty(struct g_queue *q);
void wait_n_lock(struct g_queue *q);
void unlock(struct g_queue *q);
