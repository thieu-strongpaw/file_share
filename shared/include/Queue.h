#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>	
#include <stdio.h>

struct Queue {
	int *queue;
	int head;
	int tail;
	int queue_len;
	int overflow_flag;
};

bool is_full(struct Queue *q);

bool is_empty(struct Queue *q);

struct Queue* create_queue(int len);

void print_q(struct Queue *q);

void enqueue(struct Queue *q, int x);

int dequeue(struct Queue *q);

void free_queue(struct Queue *q);

#endif //Queue.h 
