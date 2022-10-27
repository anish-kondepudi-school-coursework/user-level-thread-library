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

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/4 */
}

void uthread_yield(void)
{
	uthread_tcb_t tcb;
	queue_dequeue(queue, (void**)&tcb);
	queue_enqueue(queue, (void*)tcb);

	uthread_ctx_t* previous_ctx = current_ctx;
	current_ctx = &tcb->ctx;
	uthread_ctx_switch(previous_ctx, current_ctx);
}

void uthread_exit(void)
{
	printf("Exiting\n");
	assert(queue_length(queue) > 0);

	queue_iterate(queue, iterator_delete_tcb);
	if (queue_length(queue) == 0) {
		printf("Queue empty, returning main\n");
		// make curr_ctx main_ctx
		uthread_ctx_switch(current_ctx, main_ctx);
	}
	else {
		printf("Queue not empty\n");

		uthread_tcb_t tcb;

		assert(queue_dequeue(queue, (void**)&tcb) == 0);
		assert(queue_enqueue(queue, (void*)tcb) == 0);

		uthread_ctx_t* previous_ctx = current_ctx;
		current_ctx = &tcb->ctx;

		uthread_ctx_switch(previous_ctx, current_ctx);
	}
}

int uthread_create(uthread_func_t func, void *arg)
{
	printf("Creating new thread\n");
	uthread_tcb_t tcb = (uthread_tcb_t) malloc(sizeof(struct uthread_tcb));
	tcb->stack = uthread_ctx_alloc_stack();
	if (tcb->stack == NULL) {
		printf("Error unable to allocate space for stack\n");
		return -1;
	}
	uthread_ctx_init(&tcb->ctx, tcb->stack, func, arg);

	if (queue_enqueue(queue, tcb) == -1) {
		printf("Error enqueueing\n");
		return -1;
	}

	uthread_ctx_t* previous_ctx = current_ctx;
	current_ctx = &tcb->ctx;
	uthread_ctx_switch(previous_ctx, current_ctx);

	return 0;
}

int uthread_start(uthread_func_t func, void *arg)
{
	if (queue == NULL) {
		queue = queue_create();
		main_ctx = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
		current_ctx = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));

		if (queue == NULL) {
			printf("Error while creation queue\n");
			return -1;
		}
	}

	uthread_tcb_t tcb = (uthread_tcb_t) malloc(sizeof(struct uthread_tcb));
	tcb->stack = uthread_ctx_alloc_stack();
	if (tcb->stack == NULL) {
		printf("Error unable to allocate space for stack\n");
		return -1;
	}

	uthread_ctx_init(&tcb->ctx, tcb->stack, func, arg);

	if (queue_enqueue(queue, tcb) == -1) {
		printf("Error enqueueing\n");
		return -1;
	}
	printf("Main ctx start\n");
	current_ctx = &tcb->ctx;
	uthread_ctx_switch(main_ctx, current_ctx);
	printf("Main ctx finish\n");
	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	return uthread_start(func, arg);
}

void uthread_block(void)
{
	/* TODO Phase 4 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 4 */
}

