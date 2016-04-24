CC		= gcc

CFLAGS		= -g -Wall -pedantic

LDFLAGS		= -g

OUT		= main

OBJS		= test.c malloc_ll.c

main: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(OUT)

clean:
	rm -f *.o main core out
