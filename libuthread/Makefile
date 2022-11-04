target	:= libuthread.a
objs	:= queue.o uthread.o sem.o preempt.o context.o
CC	:= gcc
AR	:= ar
RM	:= rm
CFLAGS	:= -Wall -Werror -Wextra -g
ARFLAGS	:= rcs
RMFLAGS	:= -f

all: $(target)

deps := $(patsubst %.o,%.d,$(objs))
	-include $(deps)

$(target): $(objs)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(RMFLAGS) $(target) $(objs) $(deps)