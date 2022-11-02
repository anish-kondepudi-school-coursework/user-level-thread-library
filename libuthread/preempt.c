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
bool preempt_active = false;
bool signals_paused = false;

int set_alarm_timer(void) {
	struct itimerval value;

	value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 0;

	value.it_value.tv_sec = 0;
    value.it_value.tv_usec = HZ * MILLI_TO_MICRO_CONVERSION_CONSTANT;

	setitimer(ITIMER_VIRTUAL, &value, NULL);
}

void alarm_handler(int signum) {
	if (!preempt_active || signals_paused) {
		return;
	}
	printf("\nBeep, beep, beep, we in the alarmmmmm!\n");
	set_alarm_timer();
}

void preempt_disable(void) {
	if (!preempt_active || signals_paused) {
		return;
	}
	signals_paused = true;
}

void preempt_enable(void) {
	if (!preempt_active || !signals_paused) {
		return;
	}
	signals_paused = false;
	set_alarm_timer();
}

void preempt_start(bool preempt) {
	preempt_active = preempt;
	if (!preempt_active) {
		return;
	}

	signals_paused = false;
	sa.sa_handler = alarm_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGVTALRM, &sa, NULL);
	set_alarm_timer();
}

void preempt_stop(void) {
	if (!preempt_active) {
		return;
	}

	preempt_active = false;
}

