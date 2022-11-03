#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	queue_t queue;
	size_t count;
};

sem_t sem_create(size_t count) {
	// Allocate memory for semaphore
	sem_t sem = (sem_t) malloc(sizeof(struct semaphore));
	if (sem == NULL) {
		return NULL;
	}

	// Initialize semaphore queue
	sem->queue = queue_create();
	if (sem->queue == NULL) {
		return NULL;
	}

	// Initialize semaphore count
	sem->count = count;
	return sem;
}

int sem_destroy(sem_t sem) {
	// Terminate early if semaphore NULL or queue non-empty
	if (sem == NULL || queue_length(sem->queue) != 0) {
		return -1;
	}

	// Terminate early if queue destroy fails
	if (queue_destroy(sem->queue) == -1) {
		return -1;
	}

	// Free semaphore
	free(sem);
	return 0;
}

int sem_down(sem_t sem) {
	// Terminate early if semaphore is NULL
	if (sem == NULL) {
		return -1;
	}

	// Disable preemption before critical section
	preempt_disable();

	// Find current thread and add to queue
	struct uthread_tcb* current_tcb = uthread_current();
	queue_enqueue(sem->queue, current_tcb);

	// If no resources available, allow preemption,
	// block self, and yield to next free thread
	while (sem->count == 0) {
		preempt_enable();
		uthread_block();
		preempt_disable();
	}

	// Decrement semaphore count
	sem->count--;

	// Enabled preemption on completion of critical section
	preempt_disable();

	return 0;
}

int sem_up(sem_t sem) {
	// Terminate early if semaphore is NULL
	if (sem == NULL) {
		return -1;
	}

	// Disable preemption before critical section
	preempt_disable();

	// Increment Sem Count
	sem->count++;

	// If there are threads blocked by this sem, unblock them
	if (queue_length(sem->queue) > 0) {
		struct uthread_tcb* tcb;
		assert(queue_dequeue(sem->queue, (void**) &tcb) != -1);
		uthread_unblock(tcb);
	}

	// Enable preemption on completion of critical section
	preempt_enable();

	return 0;
}

