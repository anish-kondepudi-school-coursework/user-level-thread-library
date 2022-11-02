#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	queue_t queue;
	size_t count;
	volatile int lock;
};

void spinlock_lock(volatile int* lock) {
	while (__sync_lock_test_and_set(lock, 1)) {}
}

void spinlock_unlock(volatile int* lock) {
	__sync_synchronize();
	*lock = 0;
}

sem_t sem_create(size_t count) {
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

int sem_destroy(sem_t sem) {
	if (sem == NULL || queue_length(sem->queue) != 0) {
		return -1;
	}

	if (queue_destroy(sem->queue) == -1) {
		return -1;
	}

	free(sem);
	return 0;
}

int sem_down(sem_t sem) {
	if (sem == NULL) {
		return -1;
	}

	/* spinlock_lock(&sem->lock); */
	preempt_disable();
	struct uthread_tcb* current_tcb = uthread_current();
	queue_enqueue(sem->queue, current_tcb);
	while (sem->count == 0) {
		preempt_enable();
		uthread_block();
		preempt_disable();

	}
	preempt_enable();
	sem->count--;
	preempt_disable();
	/* spinlock_unlock(&sem->lock); */

	return 0;
}

int sem_up(sem_t sem) {
	if (sem == NULL) {
		return -1;
	}

	preempt_disable();
	sem->count++;
	if (queue_length(sem->queue) > 0) {
		struct uthread_tcb* tcb;
		assert(queue_dequeue(sem->queue, (void**) &tcb) != -1);
		uthread_unblock(tcb);
	}
	preempt_enable();

	return 0;
}

