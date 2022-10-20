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

void test_invalid_null_input_queue_destroy()
{
	fprintf(stderr, "*** TEST test_invalid_null_input_queue_destroy ***\n");

	TEST_ASSERT(queue_destroy(NULL) == -1);
}

void test_invalid_full_queue_input_queue_destroy()
{
	fprintf(stderr, "*** TEST test_invalid_full_queue_input_queue_destroy ***\n");

	int data = 3;
	queue_t q = queue_create();
	queue_enqueue(q, &data);
	TEST_ASSERT(queue_destroy(q) == -1);
}

void test_valid_inputs_queue_destroy()
{
	fprintf(stderr, "*** TEST test_valid_inputs_queue_destroy ***\n");

	queue_t q = queue_create();
	TEST_ASSERT(queue_destroy(q) == 0);
}

void test_invalid_null_queue_input_queue_enqueue()
{
	fprintf(stderr, "*** TEST test_invalid_null_queue_input_queue_enqueue ***\n");

	int data = 3;
	TEST_ASSERT(queue_enqueue(NULL, &data) == -1);
}

void test_invalid_null_data_input_queue_enqueue()
{
	fprintf(stderr, "*** TEST test_invalid_null_data_input_queue_enqueue ***\n");

	queue_t q = queue_create();
	TEST_ASSERT(queue_enqueue(q, NULL) == -1);
}

void test_valid_queue_enqueue()
{
	fprintf(stderr, "*** TEST test_queue_enqueue ***\n");

	int item = 3;
	queue_t q = queue_create();
	TEST_ASSERT(queue_enqueue(q, &item) == 0);
}

int main(void)
{
	test_create_queue();
	test_simple_enqueue_dequeue();
	test_invalid_null_input_queue_destroy();
	test_invalid_full_queue_input_queue_destroy();
	test_valid_inputs_queue_destroy();
	test_invalid_null_queue_input_queue_enqueue();
	test_invalid_null_data_input_queue_enqueue();
	test_valid_queue_enqueue();


	return 0;
}
