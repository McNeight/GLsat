# This sample Makefile allows you to make a OpenGL application
#   whose source is exactly one .c file.
#
#
# To use this Makefile, you must type:
#
#        make PROG=xxxx
#                  ----
# where
#       xxxx.c is the name of the file you wish to compile, or
#       ----

CC = gcc
# added -ltiff
LDLIBS = -I/usr/local/include \
	 -L/usr/local/lib \
	 -lglut  -lGLU -lGL 

executable: 
	$(CC) $(PROG).c \
	    $(LDLIBS) -o $(PROG)

