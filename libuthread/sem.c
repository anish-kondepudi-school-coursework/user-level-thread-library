#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	queue_t queue;
	size_t count;
	int lock;
};

void spinlock_lock(int *lock)
{
	while (test_and_set(lock) == 1);
}

void spinlock_unlock(int *lock)
{
	*lock = 0;
}

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
	sem->lock = 0;
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

}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
}

