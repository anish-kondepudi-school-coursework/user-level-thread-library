#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include <stdio.h>

struct queue {
	unsigned front, back, size, capacity;
	void** array;
};

queue_t queue_create(void)
{
	queue_t queue = (queue_t) malloc(sizeof(struct queue));
	void** array =  (void**) malloc(queue->capacity * sizeof(void*));

	if (queue == NULL || array == NULL) {
		return NULL;
	}



	return queue;
}

int queue_destroy(queue_t queue)
{
	if (queue == NULL || !is_empty(queue)) {
		return -1;
	}

	free(queue->array);
	free(queue);

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}



	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || is_empty(queue)) {
		return -1;
	}



	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}


	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	return 0;
}

int queue_length(queue_t queue)
{
	return -1;
}