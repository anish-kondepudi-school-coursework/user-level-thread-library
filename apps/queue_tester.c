#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/queue.h"

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create_queue(void)
{
	fprintf(stderr, "*** TEST queue_create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_simple_enqueue_dequeue(void)
{
	fprintf(stderr, "*** TEST test_simple_enqueue_dequeue ***\n");

	int data = 3, *ptr;
	queue_t q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

// check invalid arguments
// check edge cases with lot of arguments when queue completely full
// check if free queue works

void test_invalid_inputs_queue_destroy()
{
	fprintf(stderr, "*** TEST test_invalid_inputs_queue_destroy ***\n");

	TEST_ASSERT(queue_destroy(NULL) == -1);
}

int main(void)
{
	test_create_queue();
	test_simple_enqueue_dequeue();
	test_invalid_inputs_queue_destroy();

	return 0;
}
