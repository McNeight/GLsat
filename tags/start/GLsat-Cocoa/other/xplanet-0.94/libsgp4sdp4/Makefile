# Generated automatically from Makefile.in by configure.
prefix = /usr/local

CC = gcc
CPPFLAGS += -Wall -O3 -INONE/include -I${prefix}/include 

SRCS = \
	sgp4sdp4.c \
	sgp_in.c \
	sgp_math.c \
	sgp_obs.c \
	sgp_time.c \
	solar.c

OBJS = $(SRCS:.c=.o)

libsgp4sdp4.a: $(OBJS)
	/usr/bin/ar ruv $@ $(OBJS)
	/usr/bin/ranlib $@

ephem : main.o libsgp4sdp4.a
	$(CC) -lm -o $@ $^ 

$(OBJS) : sgp4sdp4.h

.PHONY : clean distclean
clean  :
	-/bin/rm -f *.o libsgp4sdp4.a ephem

distclean:
	-/bin/rm -f *.o config.* libsgp4sdp4.a ephem Makefile
