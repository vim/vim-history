# The most simplistic Makefile, for Cygnus gcc on MS-DOS

CFLAGS = -O2 -Wall

xxd.exe: xxd.c
	gcc $(CFLAGS) -s -o xxd.exe xxd.c -lc

clean:
	del xxd.exe
