#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	queue_t queue;
	size_t count;
};

sem_t sem_create(size_t count)
{
	sem_t sem = (sem_t) malloc(sizeof(struct semaphore));
	if (sem == NULL) {
		return NULL;
	}

	sem->queue = queue_create();
	if (sem->queue == NULL) {
		return NULL;
	}

	sem->count = count;
	return sem;
}

int sem_destroy(sem_t sem)
{
	if (sem == NULL || queue_length(sem->queue) != 0) {
		return -1;
	}

	if (queue_destroy(sem->queue) == -1) {
		return -1;
	}

	free(sem);
	return 0;
}

int sem_down(sem_t sem)
{
	/* TODO Phase 3 */
}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
}

