#include "queue.h"
#include "sched.h"
#include <pthread.h>
#define MLQ_SCHED
#ifndef MAX_PRIO
#define MAX_PRIO 140
#endif
#define MAX_QUEUE_SIZE 10
#include <stdlib.h>
#include <stdio.h>
static pthread_t _scheduler;

static pthread_mutex_t queue_lock;
static struct queue_t run_queue;
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

// int queue_empty(void)
// {
// #ifdef MLQ_SCHED
// 	unsigned long prio;
// 	for (prio = 0; prio < MAX_PRIO; prio++)
// 		if (!empty(&ready_queue.queue[prio]))
// 			return -1;
// #else
// 	return (empty(&ready_queue) && empty(&run_queue));
// }
#ifdef MLQ_SCHED
static void *sched_routine(void *args)
{
	struct pcb_t *proc = NULL;
	int prio;
	while (1)
	{
		for (prio = 0; prio < MAX_PRIO; prio++)
		{
			while (!empty(&(ready_queue.queue[prio])))
			{
				if (run_queue.size >= MAX_QUEUE_SIZE)
					continue; // Run queue is full

				// Dequeue MLQ, should be separate function but I'm Lazy. Then put it in the run queue.
				pthread_mutex_lock(&queue_lock);
				proc = dequeue(&(ready_queue.queue[prio]));
				ready_queue.size--;

				enqueue(&run_queue, proc);
				pthread_mutex_unlock(&queue_lock);
			}
		}
		pthread_testcancel();
	}
	pthread_exit(NULL);
}
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
	pthread_create(&_scheduler, NULL, sched_routine, NULL);
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
	/*TODO: get a process from running queue
	 * Remember to use lock to protect the queue.
	 * */

	pthread_mutex_lock(&queue_lock);
	proc = dequeue(&run_queue);
	pthread_mutex_unlock(&queue_lock);

	return proc;
}

void put_mlq_proc(struct pcb_t *proc)
{
	// Put process to running queue
	pthread_mutex_lock(&queue_lock);
	if (proc->prio >= 0 && proc->prio < MAX_PRIO)
		enqueue(&ready_queue.queue[proc->prio], proc);
	else
		enqueue(&ready_queue.queue[proc->priority], proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_mlq_proc(struct pcb_t *proc)
{
	// Add process to ready queue
	pthread_mutex_lock(&queue_lock);
	if (proc->prio >= 0 && proc->prio < MAX_PRIO)
		enqueue(&ready_queue.queue[proc->prio], proc);
	else
		enqueue(&ready_queue.queue[proc->priority], proc);
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

void finish_scheduler()
{
	pthread_cancel(_scheduler);
	pthread_join(_scheduler, NULL);
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
