CC=gcc
CFLAGS=-Wall -I.
DEPS=a2lib.h
OBJS=a2.o a2lib.o
EXE=a2

a2make: $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(CFLAGS)

a2.o: a2.c $(DEPS)
	$(CC) -c a2.c $(CFLAGS)

a2lib.o: a2lib.c $(DEPS)
	$(CC) -c a2lib.c $(CFLAGS)

clean:
	rm *.o
	rm $(EXE)

debug: $(OBJS)
	$(CC) -g -o $(EXE) $(OBJS) $(CFLAGS)
