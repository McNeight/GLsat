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

LDLIBS = -I/homes/tools/Mesa-3.1/include  -L/home/tools/glut-3.7/bin/glut -lglut -lGL -lGLU 

executable: 
	$(CC) $(PROG).c \
	    $(LDLIBS) -o $(PROG)

