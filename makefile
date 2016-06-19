CC		= gcc

CFLAGS		= -g -Wall -pedantic

LDFLAGS		= -g

OUT		= main

TEST		= test.c
LL			= linked.c
BUD		= buddy.c

linked: $(LL)
	$(CC) $(LDFLAGS) $(TEST) $(LL) -o $(OUT)
	./main

buddy: $(BUD)
	$(CC) $(LDFLAGS) $(TEST) $(BUD) -o $(OUT)
	./main

clean:
	rm -f *.o main

gclean:
	rm -f gawk-4.1.3/mymalloc.* gawk-4.1.3/Makefile

glinked:
	cp $(LL) gawk-4.1.3/mymalloc.c
	cp mymalloc.h gawk-4.1.3/mymalloc.h
	cp Makefile_gawk gawk-4.1.3/Makefile
	$(MAKE) -C gawk-4.1.3 check

gbuddy:
	cp $(BUD) gawk-4.1.3/mymalloc.c
	cp mymalloc.h gawk-4.1.3/mymalloc.h
	cp Makefile_gawk gawk-4.1.3/Makefile
	$(MAKE) -C gawk-4.1.3 check
