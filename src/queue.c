#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t *q)
{
	return (q->size == 0);
}

void enqueue(struct queue_t *q, struct pcb_t *proc)
{
	/* TODO: put a new process to queue [q] */
	if (q->size >= MAX_QUEUE_SIZE)
	{
		printf("Queue is full");
		return;
	}
	q->proc[q->size] = proc;
	q->size++;
}

struct pcb_t *dequeue(struct queue_t *q)
{
	// // Find process with highest priority
	// int highestPrio_index = 0;
	// for (int i = 1; i < q->size; ++i)
	// {
	// 	if (q->proc[i]->priority < q->proc[highestPrio_index]->priority)
	// 	{
	// 		highestPrio_index = i;
	// 	}
	// }

	// // Remove the process
	// struct pcb_t *proc = q->proc[highestPrio_index];
	// for (int i = highestPrio_index; i < q->size - 1; i++)
	// {
	// 	q->proc[i] = q->proc[i + 1];
	// }
	// q->size--;
	if (empty(q))
		return NULL;

	struct pcb_t *proc = q->proc[0];
	for (int i = 0; i < q->size - 1; ++i)
	{
		q->proc[i] = q->proc[i + 1];
	}
	q->size--;

	return proc;
}