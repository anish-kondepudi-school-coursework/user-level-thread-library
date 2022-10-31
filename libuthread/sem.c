#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

/* Entirety is TODO Phase 3 */
struct semaphore {
	int count;
	queue_t queue;
};

sem_t sem_create(size_t count) {
	sem_t sem = (sem_t) malloc(sizeof(struct semaphore));
	sem->count = count;
	sem->queue = queue_create();
}

int sem_destroy(sem_t sem) {
	if (sem == NULL) {
		return -1;
	}
	free(sem);
}

int sem_down(sem_t sem) {
	if (sem == NULL) {
		return -1;
	}
	if (sem->count <= 0) {
		uthread_block();
		queue_enqueue(sem->queue, sem);
		struct semaphore* sem_pointer;
		queue_gethead(sem->queue, (void**) &sem_pointer);
		while (sem->count <= 0 || sem_pointer != sem) {
			if (sem->count > 0) queue_gethead(sem->queue, (void**) &sem_pointer);
		}
		queue_dequeue(sem->queue, (void**) &sem_pointer);
	} else sem->count--;
	// successful run
	return 0;
}

int sem_up(sem_t sem) {
	if(sem == NULL) {
		return -1;
	}
	sem->count++;
	uthread_unblock(uthread_current());
}
