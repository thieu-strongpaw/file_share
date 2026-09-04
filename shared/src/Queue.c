#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Queue.h"


struct Queue* create_queue(int len){
	struct Queue *new_queue = malloc(sizeof(struct Queue));
	if( new_queue == NULL){
		fprintf(stderr, "ERROR: Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}

	new_queue->queue = malloc(len * sizeof(int));
	if(new_queue->queue == NULL){
		fprintf(stderr, "ERROR: Memory allocation failed\n");
		free(new_queue);
		exit(EXIT_FAILURE);
	}

	new_queue->queue_len = len;
	new_queue->head = -1;
	new_queue->tail = -1;
	return new_queue;
}

bool is_full(struct Queue *q){
	return ((q->tail + 1) % q->queue_len) == q->head;
}

bool is_empty(struct Queue *q){
	return q->head == -1;
}

void print_q(struct Queue *q){
	printf("Queue:\n");
	for(int i = 0; i < q->queue_len; i++){
		printf("%d:  %d\n", i+1, q->queue[i]);
	}
};


void enqueue(struct Queue *q, int x){
	if(is_full(q)){
		fprintf(stderr, "ERROR: Queue Overflow\n");
		exit(EXIT_FAILURE);
	}
	if(is_empty(q)){
		q->head = q->tail = 0;
	}else{
		q->tail = (q->tail + 1) % q->queue_len;
	}
	q->queue[q->tail] = x;
};


int dequeue(struct Queue *q){
	if(is_empty(q)){
		fprintf(stderr, "ERROR: Queue Underflow\n");
		exit(EXIT_FAILURE);
	}
	int x = q->queue[q->head];
	if(q->head == q->tail){
		q->head = q->tail = -1;
	}else{
		q->head = (q->head + 1) % q->queue_len;
	}
	return x;
};


void free_queue(struct Queue *q){
	if(q != NULL){
		if(q->queue != NULL){
			free(q->queue);
		}
		free(q);
	}
}
