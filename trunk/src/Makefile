#/*
# *  GLsat: An OpenGL-based 3D satellite tracker
# *  Copyright (C) 2002  Neil McNeight <mcneight@umich.edu>
# *
# *  This program is free software; you can redistribute it and/or modify
# *  it under the terms of the GNU General Public License as published by
# *  the Free Software Foundation; either version 2 of the License, or
# *  (at your option) any later version.
# *
# *  This program is distributed in the hope that it will be useful,
# *  but WITHOUT ANY WARRANTY; without even the implied warranty of
# *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# *  GNU General Public License for more details.
# *
# *  You should have received a copy of the GNU General Public License
# *  along with this program; if not, write to the Free Software
# *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# * 
# */

GVIM = gvim -rv -font "-adobe-courier-medium-r-normal-*-*-100-*-*-m-*-iso8859-1"
MACHINE := $(shell uname)

ifeq ("$(MACHINE)", "Linux")
  CXX = g++3
else
  CXX = g++ -I/usr/X11R6/include -I/sw/include/ -L/usr/X11R6/lib -L/sw/lib
endif

GLsat: eci.o lla.o tle.o graphics.o astro.o main.o
	$(CXX) -O3 -Wall -DNEW_FUNCTIONS -o GLsat eci.o lla.o tle.o astro.o graphics.o main.o -lm -lGL -lGLU -lglut

GLsat.exe: eci.o lla.o tle.o graphics.o astro.o main.o
	$(CXX) -O3 -Wall -DNEW_FUNCTIONS -o GLsat.exe eci.o lla.o tle.o astro.o graphics.o main.o -lm -lglut32 -lglu32 -lopengl32

text: eci.o lla.o tle.o astro.o main.o
	$(CXX) -O3 -Wall -DMATH_OUT -lm -o TeXsat eci.o lla.o tle.o astro.o main.o

debug: eci.d.o lla.d.o tle.d.o graphics.d.o astro.d.o main.d.o
	$(CXX) -g -Wall -DDEBUG -DMATH_OUT -o debug eci.d.o lla.d.o tle.d.o astro.d.o main.d.o -lm -lGL -lGLU -lglut

debug.exe: eci.d.o lla.d.o tle.d.o graphics.d.o astro.d.o main.d.o
	$(CXX) -g -Wall -DDEBUG -DMATH_OUT -o debug.exe eci.d.o lla.d.o tle.d.o astro.d.o main.d.o -lm -lglut32 -lglu32 -lopengl32

# Components

eci.d.o: eci.cpp eci.h
	$(CXX) -g -Wall -DDEBUG -c eci.cpp -o eci.d.o

eci.o: eci.cpp eci.h
	$(CXX) -Wall -O3 -c eci.cpp

lla.d.o: eci.h lla.cpp lla.h
	$(CXX) -g -Wall -DDEBUG -c lla.cpp -o lla.d.o

lla.o: eci.h lla.cpp lla.h
	$(CXX) -Wall -O3 -c lla.cpp

tle.d.o: eci.h tle.cpp tle.h
	$(CXX) -g -Wall -DDEBUG -DMATH_OUT -c tle.cpp -o tle.d.o

tle.o: eci.h tle.cpp tle.h
	$(CXX) -Wall -DNEW_FUNCTIONS -O3 -c tle.cpp

graphics.d.o: graphics.h graphics.cpp
	$(CXX) -g -Wall -DDEBUG -c graphics.cpp -o graphics.d.o

graphics.o: graphics.h graphics.cpp
	$(CXX) -Wall -O3 -c graphics.cpp

astro.d.o: astro.h astro.cpp
	$(CXX) -g -Wall -DDEBUG -c astro.cpp -o astro.d.o

astro.o: astro.h astro.cpp
	$(CXX) -Wall -O3 -c astro.cpp

main.d.o: eci.h lla.h tle.h main.cpp
	$(CXX) -g -Wall -DDEBUG -c main.cpp -o main.d.o

main.o: eci.h lla.h tle.h main.cpp
	$(CXX) -Wall -O3 -c main.cpp

clean:
	-rm -f *.o *~

transfer:
	scp -p DynHashTable.h TestHashTable.cpp xor.engin.umich.edu:~/EECS281/Project2/

start:
	$(GVIM) -geometry 100x60+0+25 main.cpp
	$(GVIM) -geometry 100x65+50+25 tle.cpp
	$(GVIM) -geometry 100x70+100+25 tle.h
#	$(GVIM) -geometry 100x70+100+25 proj2-doc.txt

length:
	wc -l *.h *.c*

# Do not edit beyond this line

