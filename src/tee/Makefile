CC=gcc
CFLAGS=-O2 -fno-strength-reduce

tee.exe: tee.o
	$(CC) $(CFLAGS) -s -o $@ $<

tee.o: tee.c
	$(CC) $(CFLAGS) -c $<
