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

uthread_ctx_t* g_main_ctx;
uthread_ctx_t* g_current_ctx;
queue_t g_queue;

typedef struct uthread_tcb* uthread_tcb_t;

struct uthread_tcb {
	uthread_ctx_t ctx;
	void* stack;
};

static void iterator_delete_tcb(queue_t queue, void* data) {
	uthread_tcb_t tcb = (uthread_tcb_t) data;
	if (&tcb->ctx == g_current_ctx) {
		assert(queue_delete(queue, data) == 0);
		uthread_ctx_destroy_stack(tcb->stack);
		free(tcb);
	}
}

uthread_tcb_t create_tcb(uthread_func_t func, void* arg) {
	// Create TCB
	uthread_tcb_t tcb = (uthread_tcb_t) malloc(sizeof(struct uthread_tcb));
	if (tcb == NULL) {
		return NULL;
	}

	// Allocate stack for TCB
	tcb->stack = uthread_ctx_alloc_stack();
	if (tcb->stack == NULL) {
		free(tcb);
		return NULL;
	}

	// Initialize TCB
	if (uthread_ctx_init(&tcb->ctx, tcb->stack, func, arg) == -1) {
		uthread_ctx_destroy_stack(tcb->stack);
		free(tcb);
		return NULL;
	}

	// Enqueue TCB
	if (queue_enqueue(g_queue, tcb) == -1) {
		uthread_ctx_destroy_stack(tcb->stack);
		free(tcb);
		return NULL;
	}

	return tcb;
}

void switch_context(uthread_ctx_t* previous_ctx, uthread_ctx_t* new_ctx) {
	g_current_ctx = new_ctx;
	uthread_ctx_switch(previous_ctx, new_ctx);
}

void uthread_yield(void) {
	// Find next TCB to switch to and move it to back of queue
	uthread_tcb_t tcb;
	assert(queue_dequeue(g_queue, (void**) &tcb) == 0);
	assert(queue_enqueue(g_queue, (void*) tcb) == 0);

	// Switch context to execute next thread
	switch_context(g_current_ctx, &tcb->ctx);
}

void uthread_exit(void) {
	// Delete current threads TCB from queue
	uthread_ctx_t current_copy_ref = *g_current_ctx;
	queue_iterate(g_queue, iterator_delete_tcb);

	// If more threads in queue, yield to next queue
	if (queue_length(g_queue) > 0) {
		uthread_yield();
		return;
	}

	// Switch context to main thread (since all threads are done)
	switch_context(&current_copy_ref, g_main_ctx);
}

int uthread_create(uthread_func_t func, void* arg) {
	// Create TCB
	uthread_tcb_t tcb = create_tcb(func, arg);
	if (tcb == NULL) {
		return -1;
	}

	return 0;
}

int uthread_start(uthread_func_t func, void* arg) {
	// Initialize global data members if needed
	bool global_data_members_uninitialized =
		g_queue == NULL &&
		g_main_ctx == NULL &&
		g_current_ctx == NULL;
	if (global_data_members_uninitialized) {
		g_queue = queue_create();
		g_main_ctx = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
		g_current_ctx = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));

		if (g_queue == NULL || g_main_ctx == NULL || g_current_ctx == NULL) {
			return -1;
		}
	}

	// Create TCB
	uthread_tcb_t tcb = create_tcb(func, arg);
	if (tcb == NULL) {
		return -1;
	}
	// Switch context to execute newly created thread
	switch_context(g_main_ctx, &tcb->ctx);

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void* arg) {
	// Making uthread_run a wrapper around uthread_star for now
	// since I'm not sure how "preempt is going to be used in phase 3
	return uthread_start(func, arg);
}

void uthread_block(void) {
	/* TODO Phase 4 */
}

void uthread_unblock(struct uthread_tcb* uthread) {
	/* TODO Phase 4 */
}

