#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue {
	unsigned front, back, size, capacity;
	int* array;
};

int is_(queue_t queue)
{
	return queue->size == queue->capacity;
}

void resize_queue(queue_t queue)
{
	int new_capacity = queue->capacity * 2;
	int* new_array = (int*) malloc(new_capacity * sizeof(int));

	for (unsigned i = queue->front; i < queue->back; i++) {
		new_array[i - queue->front] = queue->array[i];
	}

	free(queue->array);

	queue->array = new_array;
	queue->size = new_capacity / 2;
	queue->capacity = new_capacity;
	queue->front = 0;
	queue->back = new_capacity / 2 - 1;
}

queue_t queue_create(void)
{
	queue_t queue = (queue_t) malloc(sizeof(struct queue));
	int* array =  (int*) malloc(queue->capacity * sizeof(int));

	if (queue == NULL || array == NULL) {
		return NULL;
	}

	queue->array = array;
	queue->front = queue->back = 0;
	queue->capacity = queue->size = 1;

	return queue;
}

int queue_destroy(queue_t queue)
{
	if (queue == NULL || is_full(queue)) {
		return -1;
	}

	free(queue->array);
	free(queue);

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO Phase 1 */
}

int queue_dequeue(queue_t queue, void **data)
{
	/* TODO Phase 1 */
}

int queue_delete(queue_t queue, void *data)
{
	/* TODO Phase 1 */
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
	/* TODO Phase 1 */
}