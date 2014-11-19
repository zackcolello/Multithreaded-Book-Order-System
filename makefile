CC = gcc
CCFLAGS = -g

all: bookorder

bookorder: bookorder.c
	$(CC) $(CCFLAGS) -o bookorder bookorder.c

clean:
	-rm -f *.o
	-rm -f bookorder
