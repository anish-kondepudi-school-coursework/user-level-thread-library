#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include <stdio.h>

struct queue {
	unsigned front, back, size, capacity;
	void** array;
};

int is_full(queue_t queue)
{
	return queue->size == queue->capacity;
}

int is_empty(queue_t queue)
{
	return queue->size == 0;
}

int resize_queue(queue_t queue)
{
	int new_capacity = queue->capacity * 2;
	void** new_array = (void**) malloc(new_capacity * sizeof(void*));

	if (new_array == NULL) {
		return -1;
	}

	unsigned new_queue_idx = 0;
	unsigned old_queue_idx = queue->front;
	do {
		new_array[new_queue_idx++] = queue->array[old_queue_idx];
		old_queue_idx = (old_queue_idx + 1) % queue->capacity;
	} while (old_queue_idx != (queue->back + 1) % queue->capacity);

	free(queue->array);

	queue->array = new_array;
	queue->size = new_capacity / 2;
	queue->capacity = new_capacity;
	queue->front = 0;
	queue->back = new_capacity / 2 - 1;

	return 0;
}

queue_t queue_create(void)
{
	queue_t queue = (queue_t) malloc(sizeof(struct queue));
	void** array =  (void**) malloc(queue->capacity * sizeof(void*));

	if (queue == NULL || array == NULL) {
		return NULL;
	}

	queue->array = array;
	queue->front = queue->back = queue->size = 0;
	queue->capacity = 1;

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

	if (is_full(queue) && resize_queue(queue) == -1) {
		return -1;
	}

	queue->back = (queue->back + 1) % queue->capacity;
	queue->size++;
	queue->array[queue->back] = data;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || is_empty(queue)) {
		return -1;
	}

	void* item = queue->array[queue->front];
	*data = item;

	queue->size--;
	if (queue->front != queue->back) {
		queue->front = (queue->front + 1) % queue->capacity;
	}

	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	/* TODO Phase 1 */
	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
	return 0;
}

int queue_length(queue_t queue)
{
	return (queue == NULL) ? -1 : queue->size;
}