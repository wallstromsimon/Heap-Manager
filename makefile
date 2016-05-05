CC		= gcc

CFLAGS		= -g -Wall -pedantic

LDFLAGS		= -g

OUT		= main

OBJS		= test.c malloc_ll.c

LL		= test.c malloc_ll.c
LLOLD		= test.c malloc_ll_2015.c
BUD		= test.c buddy.c

linked: $(LL)
	$(CC) $(LDFLAGS) $(LL) -o $(OUT)

buddy: $(BUD)
	$(CC) $(LDFLAGS) $(BUD) -o $(OUT)

oldlinked: $(LLOLD)
	$(CC) $(LDFLAGS) $(LLOLD) -o $(OUT)

clean:
	rm -f *.o main core out

glinked:
	cp malloc_ll.c gawk-4.1.3/mymalloc.c
	cp malloc_ll.h gawk-4.1.3/mymalloc.h
	cp Makefile_gawk gawk-4.1.3/Makefile
	$(MAKE) -C gawk-4.1.3 check

gbuddy:
	cp buddy2.c gawk-4.1.3/mymalloc.c
	cp buddy.h gawk-4.1.3/mymalloc.h
	cp Makefile_gawk gawk-4.1.3/Makefile
	$(MAKE) -C gawk-4.1.3 check
