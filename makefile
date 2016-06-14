CC		= gcc

CFLAGS		= -g -Wall -pedantic

LDFLAGS		= -g

OUT		= main

TEST		= test.c
LL			= malloc_ll.c
LLOLD		= malloc_ll_old.c
BUD2		= buddy2.c

linked: $(LL)
	$(CC) $(LDFLAGS) $(TEST) $(LL) -o $(OUT)

buddy2: $(BUD2)
	$(CC) $(LDFLAGS) $(TEST) $(BUD2) -o $(OUT)
	./main

oldlinked: $(LLOLD)
	$(CC) $(LDFLAGS) $(TEST) $(LLOLD) -o $(OUT)

clean:
	rm -f *.o main

gclean:
	rm -f gawk-4.1.3/mymalloc.* gawk-4.1.3/Makefile

glinked:
	cp $(LL) gawk-4.1.3/mymalloc.c
	cp mymalloc.h gawk-4.1.3/mymalloc.h
	cp Makefile_gawk gawk-4.1.3/Makefile
	$(MAKE) -C gawk-4.1.3 check

goldlinked:
	cp $(LLOLD) gawk-4.1.3/mymalloc.c
	cp mymalloc.h gawk-4.1.3/mymalloc.h
	cp Makefile_gawk gawk-4.1.3/Makefile
	$(MAKE) -C gawk-4.1.3 check

gbuddy2:
	cp $(BUD2) gawk-4.1.3/mymalloc.c
	cp mymalloc.h gawk-4.1.3/mymalloc.h
	cp Makefile_gawk gawk-4.1.3/Makefile
	$(MAKE) -C gawk-4.1.3 check
