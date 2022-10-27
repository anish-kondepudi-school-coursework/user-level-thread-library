# Target library
lib := libuthread.a

all: $(lib)

$(lib): queue.o uthread.o preempt.o context.o
	ar rcs $@ $^

uthread.o: uthread.c uthread.h private.h queue.h
	gcc -Werror -Wextra -c $<

context.o: context.c private.h
	gcc -Wall -Werror -Wextra -c $<

preempt.o: preempt.c private.h
	gcc -Werror -Wextra -c $<

queue.o: queue.c queue.h
	gcc -Wall -Werror -Wextra -c $<

clean:
	rm -rf $(lib) *.o