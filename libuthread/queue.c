#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue {
	unsigned front, end, size, capacity;
	int* array;
};

queue_t queue_create(void)
{
	queue_t queue = (queue_t) malloc(sizeof(struct queue));

	queue->array = (int*) malloc(queue->capacity * sizeof(int));
	queue->front = queue->end = 0;
	queue->capacity = queue->size = 1;

	return queue;
}

int queue_destroy(queue_t queue)
{
	/* TODO Phase 1 */
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