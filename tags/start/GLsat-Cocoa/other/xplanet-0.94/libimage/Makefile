# Generated automatically from Makefile.in by configure.
prefix = /usr/local

CC = gcc
CPPFLAGS += -Wall -O3 -INONE/include -I${prefix}/include 

SRCS = \
	 \
	bmp.c \
	image.c 

OBJS = $(SRCS:.c=.o)

all: libimage.a

libimage.a: $(OBJS)
	/usr/bin/ar ruv $@ $(OBJS)
	/usr/bin/ranlib $@

.PHONY : clean distclean
clean:
	/bin/rm -f *.o libimage.a

distclean:
	/bin/rm -f *.o libimage.a Makefile config.* checkfuncs.h
