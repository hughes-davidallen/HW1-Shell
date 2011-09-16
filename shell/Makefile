CC := gcc
CFLAGS := -Wall -Werror -pg
LDFLAGS := 


OBJECTS := shell.o stack.o list.o

all: w4118_sh


w4118_sh: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f w4118_sh
	rm -f *.o

.PHONY: clean
