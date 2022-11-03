/*
 * Simple preempt
 *
 * Tests the if preemption works and successfully return.
 * Thread 1 runs first. It creates thread 2 but never yields
 * to it. It then halts itself in a infinite loop until
 * thread 2 completed. Thus, only once the scheduler preemptively
 * de-schedules and tread 2 will thread 1's execution finish.
 * The final result should be:
 *
 * thread2
 * thread1
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <uthread.h>

volatile bool g_thread2_executed;

void thread2(void* arg) {
    (void) arg;

    printf("thread2\n");
    g_thread2_executed = true;
}

void thread1(void* arg) {
    (void) arg;

    uthread_create(thread2, NULL);
    while (!g_thread2_executed) {
        // Halt till thread2 is preempted and completes
    }
    printf("thread1\n");
}

int main(void) {
    g_thread2_executed = false;
    assert(uthread_run(true, thread1, NULL) == 0);
    return 0;
}
