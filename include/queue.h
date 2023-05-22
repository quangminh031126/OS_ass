#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

struct pcb_node
{
	struct pcb_t *proc;
	struct pcb_node *next;
};
struct queue_t
{
	struct pcb_node *head;
	struct pcb_node *tail;
	int size;
};
void enqueue(struct queue_t *q, struct pcb_t *proc);

struct pcb_t *dequeue(struct queue_t *q);

int empty(struct queue_t *q);

#endif