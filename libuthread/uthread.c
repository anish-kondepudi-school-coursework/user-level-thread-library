#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"


uthread_ctx_t* main_ctx;
uthread_ctx_t* current_ctx;
queue_t queue;

struct uthread_tcb {
	uthread_ctx_t ctx;
	void* stack;
};

static void iterator_delete_tcb(queue_t queue, void* data) {
	uthread_tcb_t tcb = (uthread_tcb_t) data;
	if (&tcb->ctx == current_ctx) {
		assert(queue_delete(queue, data) == 0);
		free(tcb->stack);
		free(tcb);
	}
}

struct uthread_tcb* uthread_current(void) {
	/* TODO Phase 2/4 */
}

void uthread_yield(void) {
	// Find next tcb to switch to and move it to back of queue
	uthread_tcb_t tcb;
	assert(queue_dequeue(queue, (void**) &tcb) == 0);
	assert(queue_enqueue(queue, (void*) tcb) == 0);

	// Switch context to execute next thread
	uthread_ctx_t* previous_ctx = current_ctx;
	current_ctx = &tcb->ctx;
	uthread_ctx_switch(previous_ctx, current_ctx);
}

void uthread_exit(void) {
	// Delete current threads TCB from queue
	queue_iterate(queue, iterator_delete_tcb);

	// If more threads in queue, yield to next queue
	if (queue_length(queue) > 0) {
		uthread_yield();
		return;
	}

	// Switch context to main thread (since all threads are done)
	uthread_ctx_t* previous_ctx = current_ctx;
	current_ctx = main_ctx;
	uthread_ctx_switch(previous_ctx, current_ctx);
}

int uthread_create(uthread_func_t func, void* arg) {
	// Create TCB
	uthread_tcb_t tcb = (uthread_tcb_t) malloc(sizeof(struct uthread_tcb));
	if (tcb == NULL) {
		return -1;
	}

	// Allocate stack for TCB
	tcb->stack = uthread_ctx_alloc_stack();
	if (tcb->stack == NULL) {
		return -1;
	}

	// Initialize TCB
	if (uthread_ctx_init(&tcb->ctx, tcb->stack, func, arg) == -1) {
		return -1;
	}

	// Enqueue TCB
	if (queue_enqueue(queue, tcb) == -1) {
		return -1;
	}

	// Switch context to execute newly created thread
	uthread_ctx_t* previous_ctx = current_ctx;
	current_ctx = &tcb->ctx;
	uthread_ctx_switch(previous_ctx, current_ctx);

	return 0;
}

int uthread_start(uthread_func_t func, void* arg) {
	// Initialize global data members if needed
	bool global_data_members_uninitialized =
		queue == NULL &&
		main_ctx == NULL &&
		current_ctx == NULL;
	if (global_data_members_uninitialized) {
		queue = queue_create();
		main_ctx = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
		current_ctx = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));

		if (queue == NULL || main_ctx == NULL || current_ctx == NULL) {
			return -1;
		}
	}

	// Create TCB
	uthread_tcb_t tcb = (uthread_tcb_t) malloc(sizeof(struct uthread_tcb));
	if (tcb == NULL) {
		return -1;
	}

	// Allocate stack for TCB
	tcb->stack = uthread_ctx_alloc_stack();
	if (tcb->stack == NULL) {
		return -1;
	}

	// Initialize TCB
	if (uthread_ctx_init(&tcb->ctx, tcb->stack, func, arg) == -1) {
		return -1;
	}

	// Enqueue TCB
	if (queue_enqueue(queue, tcb) == -1) {
		return -1;
	}

	// Switch context to execute newly created thread
	current_ctx = &tcb->ctx;
	uthread_ctx_switch(main_ctx, current_ctx);

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void* arg) {
	return uthread_start(func, arg);
}

void uthread_block(void) {
	/* TODO Phase 4 */
}

void uthread_unblock(struct uthread_tcb* uthread) {
	/* TODO Phase 4 */
}

