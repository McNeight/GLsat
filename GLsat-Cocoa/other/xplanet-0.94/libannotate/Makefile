# Generated automatically from Makefile.in by configure.
prefix = /usr/local

CXX = c++
CPPFLAGS += -Wall -O3 -INONE/include -I${prefix}/include -I..
CXXFLAGS += -I/usr/X11R6/include -I/usr/X11R6/include/freetype2  -I/usr/X11R6/include

SRCS = \
	drawGreatArcs.cc 	\
	drawMarkers.cc 		\
	drawSatellite.cc 	\
	parse.cc

OBJS = $(SRCS:.cc=.o)

libannotate.a: $(OBJS)
	/usr/bin/ar ruv $@ $(OBJS)
	/usr/bin/ranlib $@

drawGreatArcs.o: drawGreatArcs.cc ../Options.h ../checkfuncs.h \
 ../keywords.h parse.h ../Planet.h ../util.h \
 ../libdisplay/libdisplay.h ../libdisplay/DisplayBase.h \
 ../libprojection/libprojection.h ../libprojection/ProjectionBase.h \
 ../libprojection/ProjectionRectangular.h ../Location.h

drawMarkers.o: drawMarkers.cc ../auxfiles.h ../Marker.h ../Options.h \
 ../checkfuncs.h ../keywords.h parse.h ../Planet.h ../util.h \
 ../xplanet.h ../libdisplay/libdisplay.h ../libdisplay/DisplayBase.h \
 ../libprojection/libprojection.h ../libprojection/ProjectionBase.h \
 ../libprojection/ProjectionRectangular.h ../Location.h

drawSatellite.o: drawSatellite.cc ../auxfiles.h ../Location.h \
 ../Options.h ../checkfuncs.h ../keywords.h parse.h ../Satellite.h \
 ../libsgp4sdp4/sgp4sdp4.h ../util.h

parse.o: parse.cc ../keywords.h

.PHONY : clean distclean

clean  :
	-/bin/rm -f *.o libannotate.a

distclean:
	-/bin/rm -f *.o libannotate.a Makefile
