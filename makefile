CC		= gcc

CFLAGS		= -g -Wall -pedantic

LDFLAGS		= -g

OUT		= main

OBJS		= test.c malloc_ll.c

LL		= test.c malloc_ll.c
BUD		= test.c malloc_ll.c


main: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(OUT)

linked: $(LL)
	$(CC) $(LDFLAGS) $(LL) -o $(OUT)

buddy: $(BUD)
	$(CC) $(LDFLAGS) $(BUD) -o $(OUT)

clean:
	rm -f *.o main core out

glinked:
	cp malloc_ll.c gawk-4.1.3/mymalloc.c
	cp malloc_ll.h gawk-4.1.3/mymalloc.h
	cp Makefile_gawk gawk-4.1.3/Makefile
	$(MAKE) -C gawk-4.1.3 check
