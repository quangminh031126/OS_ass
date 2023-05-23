#include "queue.h"
#include "sched.h"
#include <pthread.h>
#define MLQ_SCHED
#ifndef MAX_PRIO
#define MAX_PRIO 25
#endif
#include <stdlib.h>
#include <stdio.h>
static struct queue_t run_queue;
static pthread_mutex_t queue_lock;

#ifdef MLQ_SCHED
struct mlq_ready_queue
{
	struct queue_t queue[MAX_PRIO];
	unsigned int size;
};
static struct mlq_ready_queue ready_queue;
#else
static struct queue_t ready_queue;
#endif

void init_scheduler(void)
{
#ifdef MLQ_SCHED
	int i;
	for (i = 0; i < MAX_PRIO; i++)
		init_queue(&(ready_queue.queue[i]));
#endif
	ready_queue.size = 0;
	run_queue.size = 0;
	pthread_mutex_init(&queue_lock, NULL);
}
int mlq_empty(struct mlq_ready_queue *mlq)
{
	return (mlq->size == 0);
}

#ifdef MLQ_SCHED
/*
 *  Stateful design for routine calling
 *  based on the priority and our MLQ policy
 *  We implement stateful here using transition technique
 *  State representation   prio = 0 .. MAX_PRIO, curr_slot = 0..(MAX_PRIO - prio)
 */
struct pcb_t *get_mlq_proc(void)
{
	struct pcb_t *proc = NULL;
	/*TODO: get a process from PRIORITY [ready_queue].
	 * Remember to use lock to protect the queue.
	 * */

	pthread_mutex_lock(&queue_lock);
	if (!mlq_empty(&ready_queue))
	{
		for (int i = 0; i < MAX_PRIO; ++i)
		{
			if (!empty(&ready_queue.queue[i]))
			{
				proc = dequeue(&ready_queue.queue[i]);
				ready_queue.size--;
				break;
			}
		}
	}
	pthread_mutex_unlock(&queue_lock);

	return proc;
}

void put_mlq_proc(struct pcb_t *proc)
{
	if (!proc)
		return;
	pthread_mutex_lock(&queue_lock);
	if (proc->prio != UINT32_MAX)
		enqueue(&ready_queue.queue[proc->prio], proc);
	else
		enqueue(&ready_queue.queue[proc->priority], proc);
	ready_queue.size++;
	pthread_mutex_unlock(&queue_lock);
}

void add_mlq_proc(struct pcb_t *proc)
{
	if (!proc)
		return;
	pthread_mutex_lock(&queue_lock);
	if (proc->prio != UINT32_MAX)
		enqueue(&ready_queue.queue[proc->prio], proc);
	else
		enqueue(&ready_queue.queue[proc->priority], proc);
	ready_queue.size++;
	pthread_mutex_unlock(&queue_lock);
}

struct pcb_t *get_proc(void)
{
	return get_mlq_proc();
}

void put_proc(struct pcb_t *proc)
{
	return put_mlq_proc(proc);
}

void add_proc(struct pcb_t *proc)
{
	return add_mlq_proc(proc);
}
#else
struct pcb_t *get_proc(void)
{
	struct pcb_t *proc = NULL;
	/*TODO: get a process from [ready_queue].
	 * Remember to use lock to protect the queue.
	 * */
	if (!empty(&ready_queue))
	{
		proc = dequeue(&ready_queue);
		return proc;
	}
	else
		return NULL;
}
void put_proc(struct pcb_t *proc)
{
	pthread_mutex_lock(&queue_lock);
	enqueue(&run_queue, proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_proc(struct pcb_t *proc)
{
	pthread_mutex_lock(&queue_lock);
	enqueue(&ready_queue, proc);
	pthread_mutex_unlock(&queue_lock);
}
#endif
