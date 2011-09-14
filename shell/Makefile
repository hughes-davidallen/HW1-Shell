CC := gcc
CFLAGS := -Wall -Werror
LDFLAGS := 


OBJECTS := shell.o stack.o

all: w4118_sh


w4118_sh: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f w4118_sh stack_test
	rm -f *.o

.PHONY: clean

stack_test: stack_test.o stack.o
	$(CC) $(CFLAGS) -o stack_test stack_test.o stack.o
stack_test.o: stack_test.c stack.h
	$(CC) $(CFLAGS) -c stack_test.c
stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c
