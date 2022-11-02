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

enum State {
	Idle,
	Ready,
	Blocked,
	Running
};

typedef struct uthread_tcb* uthread_tcb_t;

struct uthread_tcb {
	uthread_ctx_t ctx;
	enum State state;
	void* stack;
};

uthread_tcb_t g_current_tcb;
queue_t g_queue;

static void iterator_delete_tcb(queue_t queue, void* data) {
	uthread_tcb_t tcb = (uthread_tcb_t) data;
	if (&tcb->ctx == &g_current_tcb->ctx) {
		assert(queue_delete(queue, data) == 0);
		uthread_ctx_destroy_stack(tcb->stack);
		free(tcb);
	}
}

struct uthread_tcb* uthread_current(void) {
	return g_current_tcb;
}

uthread_tcb_t create_and_enqueue_tcb(uthread_func_t func, void* arg) {
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

	// Set TCB state to ready
	tcb->state = Ready;
	return tcb;
}

void switch_context(uthread_tcb_t previous_tcb, uthread_tcb_t new_tcb) {
	// Update states for TCBs
	if (previous_tcb->state == Running) {
		previous_tcb->state = Ready;
	}

	if (new_tcb->state == Ready) {
		new_tcb->state = Running;
	}

	// Update global current TCB
	g_current_tcb = new_tcb;

	// Switch context
	uthread_ctx_switch(&previous_tcb->ctx, &new_tcb->ctx);
}

void uthread_yield(void) {
	// If only idle queue left, switch to it
	uthread_tcb_t tcb;
	if (queue_length(g_queue) == 1) {
		assert(queue_dequeue(g_queue, (void**) &tcb) == 0);
		switch_context(g_current_tcb, tcb);
	}

	// If more threads than just idle, find next ready TCB to switch to
	do {
		assert(queue_dequeue(g_queue, (void**) &tcb) == 0);
		assert(queue_enqueue(g_queue, (void*) tcb) == 0);
	} while (tcb->state != Ready);

	// Switch context to execute next thread
	switch_context(g_current_tcb, tcb);
}

void uthread_exit(void) {
	// Delete current threads TCB from queue
	assert(queue_length(g_queue) >= 2);
	queue_iterate(g_queue, iterator_delete_tcb);
	uthread_yield();
}

int uthread_create(uthread_func_t func, void* arg) {
	// Create TCB
	uthread_tcb_t tcb = create_and_enqueue_tcb(func, arg);
	if (tcb == NULL) {
		return -1;
	}

	return 0;
}

int uthread_start(uthread_func_t func, void* arg) {
	// Initialize global data members if needed
	if (g_queue == NULL && g_current_tcb == NULL) {
		g_queue = queue_create();
		g_current_tcb = (uthread_tcb_t) malloc(sizeof(struct uthread_tcb));

		if (g_queue == NULL || g_current_tcb == NULL) {
			return -1;
		}
	}

	// Create and Enqueue next TCB
	uthread_tcb_t tcb = create_and_enqueue_tcb(func, arg);
	if (tcb == NULL) {
		return -1;
	}

	// Create TCB for idle thread
	uthread_tcb_t idle_thread_tcb = (uthread_tcb_t) malloc(sizeof(struct uthread_tcb));
	if (idle_thread_tcb == NULL) {
		return -1;
	}
	idle_thread_tcb->state = Idle;

	// Enqueue TCB for idle thread and update global TCB
	g_current_tcb = idle_thread_tcb;
	if (queue_enqueue(g_queue, idle_thread_tcb) == -1) {
		return -1;
	}

	// Yield on idle thread until it's only process left in queue
	while (queue_length(g_queue) > 1) {
		uthread_yield();
	}

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void* arg) {
	// Making uthread_run a wrapper around uthread_star for now
	// since I'm not sure how "preempt is going to be used in phase 3
	return uthread_start(func, arg);
}

void uthread_block(void) {
	g_current_tcb->state = Blocked;
	uthread_yield();
}

void uthread_unblock(struct uthread_tcb* uthread) {
	uthread->state = Ready;
}