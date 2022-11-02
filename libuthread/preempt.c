#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define MILLI_TO_MICRO_CONVERSION_CONSTANT 1000

struct sigaction sa;

int set_alarm_timer(void) {
	struct itimerval value;

	value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 0;

	value.it_value.tv_sec = 0;
    value.it_value.tv_usec = HZ * MILLI_TO_MICRO_CONVERSION_CONSTANT;

	setitimer(ITIMER_REAL, &value, NULL);
}

void alarm_handler(int signum) {
	printf("\nBeep, beep, beep, we in the alarmmmmm!\n");
	set_alarm_timer();
}

void preempt_disable(void) {
	/* TODO Phase 4 */
}

void preempt_enable(void) {
	sa.sa_handler = alarm_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGALRM, &sa, NULL);
	set_alarm_timer();
}

void preempt_start(bool preempt) {
	/* TODO Phase 4 */
}

void preempt_stop(void) {
	/* TODO Phase 4 */
}

