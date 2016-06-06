/*
 * TCSS 422 Final Project
 * Team 4
 * Tempest Parr, Paul Zander, Geoffrey Tanay, Justin Clark
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "PCB_Priority_Queue.h"
#include "PCB_Queue.h"
#include "PCB.h"

PCB_Priority_Queue_p PCB_Priority_Queue_construct(enum PCB_ERROR *error) {
	PCB_Priority_Queue_p pq = malloc(sizeof(struct PCB_Priority_Queue));
	if (pq == NULL) {
		*error = PCB_MEM_ALLOC_FAIL;
		return NULL;
	}
	int i;
	for (i = 0; i <= PCB_PRIORITY_MAX; i++) {
		pq->queues[i] = PCB_Queue_construct(error);
		if (pq->queues[i] == NULL) {
			*error = PCB_MEM_ALLOC_FAIL;
			return NULL;
		}
	}
	return pq;
}

void PCB_Priority_Queue_enqueue(PCB_Priority_Queue_p pq, PCB_p pcb, enum PCB_ERROR *error) {
	if (pq == NULL || pcb == NULL) {
		*error = PCB_NULL_POINTER;
		return;
	}
	PCB_Queue_p queue = pq->queues[PCB_get_priority(pcb, error)];
	PCB_Queue_enqueue(queue, pcb, error);
}

PCB_p PCB_Priority_Queue_dequeue(PCB_Priority_Queue_p pq, enum PCB_ERROR *error) {
	if (pq == NULL) {
		*error = PCB_NULL_POINTER;
		return NULL;
	}
	int i = 0;
	while (PCB_Queue_is_empty(pq->queues[i], error)) {
		if (i > PCB_PRIORITY_MAX) {
			*error = PCB_INVALID_ARG;
			return NULL;
		}
		i++;
	}
	return PCB_Queue_dequeue(pq->queues[i], error);
}

void PCB_Priority_Queue_print(PCB_Priority_Queue_p pq, enum PCB_ERROR *error) {
	int i;
	for (i = 0; i <= PCB_PRIORITY_MAX; i++) {
		PCB_Queue_p q = pq->queues[i];
		printf("Q%u: Count=%u: ", i, q->size);
		struct node *n = q->first_node_ptr;
		while (n != NULL) {
			printf("->%lu", n->value->pid);
			n = n->next_node;
		}
		printf("-*\n");
	}
}

/*
 * Increases the priority of a starved process
 */
void PCB_Priority_Queue_promote(PCB_Priority_Queue_p pq, int priority, enum PCB_ERROR *error) {
	PCB_p pcb = PCB_Queue_dequeue(pq->queues[priority], error);
	//mark as having the priority boosted
	if (!pcb->priority_boost) {
		pcb->priority_boost = 1;
	}
	//increase priority and re-enqueue
	pcb->priority--; 
	pcb->starvation_quanta_count++;
	PCB_Priority_Queue_enqueue(pq, pcb, error);	
}


int PCB_Priority_Queue_is_empty(PCB_Priority_Queue_p pq, enum PCB_ERROR *error) {
	int i = 0;
	int result = 1;
	if (pq == NULL) {
		*error = PCB_NULL_POINTER;
		return -1;
	}
	for (i = 0; i <= PCB_PRIORITY_MAX; i++) {
		result *= PCB_Queue_is_empty(pq->queues[i], error);
	}
	return result;
}
