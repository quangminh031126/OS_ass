#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void init_queue(struct queue_t *q)
{
	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
}

int empty(struct queue_t *q)
{
	return (q->size == 0);
}

void enqueue(struct queue_t *q, struct pcb_t *proc)
{
	/* TODO: put a new process to queue [q] */
	if (proc == NULL)
		return;
	struct pcb_node *new_node = (struct pcb_node *)malloc(sizeof(struct pcb_node));
	new_node->proc = proc;
	new_node->next = NULL;

	if (q->tail == NULL)
	{
		q->head = new_node;
		q->tail = new_node;
	}
	else
	{
		q->tail->next = new_node;
		q->tail = new_node;
	}

	q->size++;
}

struct pcb_t *dequeue(struct queue_t *q)
{
	// Check if queue is NULL
	if (q->head == NULL)
		return NULL;
	// Queue is not NULL
	struct pcb_node *first_node = q->head;
	struct pcb_t *first_proc = first_node->proc;

	q->head = first_node->next;
	if (q->head == NULL)
		q->tail = NULL;

	free(first_node);
	q->size--;

	return first_proc;
}