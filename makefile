CC = gcc
CCFLAGS = -g

all: bookorder

bookorder: bookorder.c
	$(CC) $(CCFLAGS) -o bookorder queue.c bookorder.c -lpthread


clean:
	-rm -f *.o
	-rm -f bookorder
