#Makefile for norad     03/17/2001

SHELL = /bin/sh
CC    = gcc -Wall -O3

vpath   %.c ./src
vpath   %.h ./src

objects = norad.o main.o

norad : $(objects)
	$(CC) -lm -o norad $(objects)

$(objects) : norad.h

.PHONY : clean
clean  :
	-rm -f *.o *~

