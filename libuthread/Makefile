# Target library
lib := libuthread.a

all: $(lib)

$(lib): queue.o
	ar rcs $@ $<

queue.o: queue.c queue.h
	gcc -Wall -Werror -Wextra -c -o queue.o queue.c

clean:
	rm -rf $(lib) *.o