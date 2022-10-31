#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

/* Entirety is TODO Phase 3 */
struct semaphore {
	int count;
	int thread_id;
	queue_t thread_id_queue;
	queue_t uthread_struct_queue;
};

sem_t sem_create(size_t count) {
	sem_t sem = (sem_t) malloc(sizeof(struct semaphore));
	sem->count = count;
	sem->thread_id = 0;
	sem->thread_id_queue = queue_create();
	sem->uthread_struct_queue = queue_create();
}

int sem_destroy(sem_t sem) {
	if (sem == NULL) {
		return -1;
	}
	queue_destroy(sem->thread_id_queue);
	queue_destroy(sem->uthread_struct_queue);
	free(sem);
}

int sem_down(sem_t sem) {
	if (sem == NULL) {
		return -1;
	}
	struct uthread_tcb* current_thread = uthread_current();
	queue_enqueue(sem->thread_id_queue, current_thread);
	if (sem->count <= 0) {
		// creating local variable to store current thread id in a manner unique to thread
		int this_thread_id = sem->thread_id;
		sem->thread_id++;
		queue_enqueue(sem->thread_id_queue, this_thread_id);
		// creating local variable for determining id of the thread at the head of the queue
		int head_thread_id;
		queue_gethead(sem->thread_id_queue, (void**) &head_thread_id);
		uthread_block();
		// spin locking until a resource is available AND the current thread is at the head of the queue
		while (sem->count <= 0 || this_thread_id != head_thread_id) {
			if (sem->count > 0) {
				queue_gethead(sem->thread_id_queue, (void**) &head_thread_id);
				if (this_thread_id == head_thread_id) break;
				else {
					uthread_block();
				}
			}
		}
		queue_dequeue(sem->thread_id_queue, (void**) &head_thread_id);
	}
	else sem->count--;
	// successful run
	return 0;
}

int sem_up(sem_t sem) {
	if (sem == NULL) {
		return -1;
	}
	// unbl
	sem->count++;
	uthread_unblock(uthread_current());
	queue_gethead()

	// context switch here from the head of the queue
}
