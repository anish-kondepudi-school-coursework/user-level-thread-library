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


uthread_ctx_t main_ctx;
queue_t queue;

struct uthread_tcb {
	uthread_ctx_t ctx;
	void* stack;
};

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/4 */
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	return 0;
}

int uthread_start(uthread_func_t func, void *arg)
{
	if (queue == NULL) {
		queue = queue_create();

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

	uthread_ctx_switch(&main_ctx, &(tcb->ctx));

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

