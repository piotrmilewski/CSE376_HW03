CC= gcc 
CFLAGS= -g -O2 -Wall -Werror 
LDFLAGS= -lm 
OBJS= smash.o exec.o parse.o jobs.o redirect.o debug.o

all: smash

allTests: all
	./allTests.sh

test1: all
	./test1.sh

test2: all
	./test2.sh

test3: all
	./test3.sh

clean:
	rm -f *.o *.a *.d smash forTesting/newErrfile forTesting/newOutfile forTesting/sorted.txt

smash: $(OBJS)
	$(CC) -o smash $(CFLAGS) $(OBJS) $(LDFLAGS)

smash.o: smash.c smash.h
	$(CC) -c $(CFLAGS) smash.c

parse.o: parse.c parse.h
	$(CC) -c $(CFLAGS) parse.c

exec.o: exec.c exec.h
	$(CC) -c $(CFLAGS) exec.c

jobs.o: jobs.c jobs.h
	$(CC) -c $(CFLAGS) jobs.c

redirect.o: redirect.c redirect.h
	$(CC) -c $(CFLAGS) redirect.c

debug.o: debug.c debug.h
	$(CC) -c $(CFLAGS) debug.c
