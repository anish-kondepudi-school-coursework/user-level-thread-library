#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/queue.h"

#define TEST_ASSERT(assert)					\
do {										\
	if (assert) {							\
		printf("PASS");						\
	} else	{								\
		printf("FAIL");						\
		exit(1);							\
	}										\
	printf("\tASSERT: " #assert "\n");		\
} while(0)									\

void test_create_queue(void)
{
	fprintf(stderr, "\n*** TEST queue_create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

void test_simple_enqueue_dequeue(void)
{
	fprintf(stderr, "\n*** TEST test_simple_enqueue_dequeue ***\n");

	int data = 3, *ptr;
	queue_t q = queue_create();
	TEST_ASSERT(queue_enqueue(q, &data) == 0);
	TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == 0);
	TEST_ASSERT(ptr == &data);
}

void test_invalid_null_input_queue_destroy()
{
	fprintf(stderr, "\n*** TEST test_invalid_null_input_queue_destroy ***\n");

	TEST_ASSERT(queue_destroy(NULL) == -1);
}

void test_invalid_full_queue_input_queue_destroy()
{
	fprintf(stderr, "\n*** TEST test_invalid_full_queue_input_queue_destroy ***\n");

	int data = 3;
	queue_t q = queue_create();
	queue_enqueue(q, &data);
	queue_enqueue(q, &data);
	TEST_ASSERT(queue_destroy(q) == -1);
}

void test_valid_inputs_queue_destroy()
{
	fprintf(stderr, "\n*** TEST test_valid_inputs_queue_destroy ***\n");

	queue_t q = queue_create();
	TEST_ASSERT(queue_destroy(q) == 0);
}

void test_invalid_null_queue_input_queue_enqueue()
{
	fprintf(stderr, "\n*** TEST test_invalid_null_queue_input_queue_enqueue ***\n");

	int data = 3;
	TEST_ASSERT(queue_enqueue(NULL, &data) == -1);
}

void test_invalid_null_data_input_queue_enqueue()
{
	fprintf(stderr, "\n*** TEST test_invalid_null_data_input_queue_enqueue ***\n");

	queue_t q = queue_create();
	TEST_ASSERT(queue_enqueue(q, NULL) == -1);
}

void test_invalid_null_queue_input_queue_dequeue()
{
	fprintf(stderr, "\n*** TEST test_invalid_null_queue_input_queue_dequeue ***\n");

	int* data;
	TEST_ASSERT(queue_dequeue(NULL, (void**)&data) == -1);
}

void test_invalid_null_data_input_queue_dequeue()
{
	fprintf(stderr, "\n*** TEST test_invalid_null_data_input_queue_dequeue ***\n");

	queue_t q = queue_create();
	TEST_ASSERT(queue_dequeue(q, NULL) == -1);
}

void test_empty_queue_dequeue()
{
	fprintf(stderr, "\n*** TEST test_empty_queue_dequeue ***\n");

	int *data;
	queue_t q = queue_create();
	TEST_ASSERT(queue_dequeue(q, (void**)&data) == -1);
}

void test_invalid_null_input_queue_length()
{
	fprintf(stderr, "\n*** TEST test_invalid_null_input_queue_length ***\n");
	TEST_ASSERT(queue_length(NULL) == -1);
}

void test_simple_queue_length()
{
	fprintf(stderr, "\n*** TEST test_simple_queue_length ***\n");

	int data = 3;
	queue_t q = queue_create();
	for (int i = 0; i < 100; i++) {
		queue_enqueue(q, &data);
	}

	int* ptr;
	for (int i = 0; i < 50; i++) {
		queue_dequeue(q, (void**)&ptr);
	}

	TEST_ASSERT(queue_length(q) == 50);
}

void test_empty_queue_length()
{
	fprintf(stderr, "\n*** test_empty_queue_length ***\n");

	int data, *ptr;
	queue_t q = queue_create();
	TEST_ASSERT(queue_length(q) == 0);

	queue_enqueue(q, &data);
	queue_enqueue(q, &data);
	TEST_ASSERT(queue_length(q) == 2);

	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(queue_length(q) == 1);

	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(queue_length(q) == 0);

	queue_enqueue(q, &data);
	TEST_ASSERT(queue_length(q) == 1);
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
	test_invalid_null_queue_input_queue_dequeue();
	test_invalid_null_data_input_queue_dequeue();
	test_empty_queue_dequeue();
	test_invalid_null_input_queue_length();
	test_simple_queue_length();
	test_empty_queue_length();


	return 0;
}
