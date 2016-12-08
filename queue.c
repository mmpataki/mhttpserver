
#include "queue.h"

struct queue_ent *new_queue_ent(void *data)
{
	struct queue_ent *nqe = (struct queue_ent*)
	malloc(sizeof(struct queue_ent));
	if(nqe) {
		nqe->data = data;
		nqe->next = NULL;
	}
	return nqe;
}

void enque(struct g_queue *q, void *data)
{
	wait_n_lock(q);

	struct queue_ent **indirect = &q->last;	
	struct queue_ent **lind = indirect;

	if(q->first == NULL) {
		indirect = &q->first ;
	}
	if(q->last != NULL) {
		lind = &q->last->next;
	}

	*indirect = q->last = *indirect = new_queue_ent(data);
	q->cnt++;

	unlock(q);
}

void *deque(struct g_queue *q)
{
	wait_n_lock(q);

	if(!q || !q->first) {
		unlock(q);
		return NULL;
	}

	struct queue_ent *temp, **indirect;
	void *data;

	temp = q->first;
	indirect = &q->first;

	if(q->first == q->last) {
		indirect = &q->last;
	}
	*indirect = q->first = q->first->next;
	q->cnt--;

	unlock(q);

	data = temp->data;
	free(temp);
	return data;
}

int isempty(struct g_queue *q)
{
	int val;

	wait_n_lock(q);
	val = !(long long int)q->first;//(q->cnt);
	unlock(q);
	return val;
}

void wait_n_lock(struct g_queue *q)
{
	while(q->lock != 0);
	q->lock = 1;
}

void unlock(struct g_queue *q)
{
	q->lock = 0;

	/*
	 * wait for a while until the other thread
	 * can acquire lock if it wishes
	 */
	int delay = 2000;
	while(delay--);
}
