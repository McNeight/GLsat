# Generated automatically from Makefile.in by configure.
prefix = /usr/local

CXX = c++
CPPFLAGS += -Wall -O3 -INONE/include -I${prefix}/include -I..  -I/usr/X11R6/include

SRCS = \
	getColor.cc \
	getProjection.cc \
	ProjectionBase.cc \
	ProjectionAncient.cc \
	ProjectionAzimuthal.cc \
	ProjectionHemisphere.cc \
	ProjectionMercator.cc \
	ProjectionMollweide.cc \
	ProjectionOrthographic.cc \
	ProjectionPeters.cc \
	ProjectionRectangular.cc \

OBJS = $(SRCS:.cc=.o)

libprojection.a: $(OBJS)
	/usr/bin/ar ruv $@ $(OBJS)
	/usr/bin/ranlib $@

getColor.o: getColor.cc ../Location.h ../Options.h ../checkfuncs.h \
 ../keywords.h ../util.h ../xplanet.h libprojection.h ProjectionBase.h \
 ProjectionRectangular.h

getProjection.o: getProjection.cc ../keywords.h ProjectionBase.h \
 ProjectionAncient.h ProjectionAzimuthal.h ProjectionHemisphere.h \
 ProjectionMollweide.h ProjectionMercator.h ProjectionOrthographic.h \
 ProjectionPeters.h ProjectionRectangular.h ../Location.h

ProjectionAncient.o: ProjectionAncient.cc ../Options.h ../checkfuncs.h \
 ../keywords.h ProjectionAncient.h ProjectionBase.h ../util.h

ProjectionAzimuthal.o: ProjectionAzimuthal.cc ../Options.h \
 ../checkfuncs.h ../keywords.h ProjectionAzimuthal.h ProjectionBase.h \
 ../util.h

ProjectionBase.o: ProjectionBase.cc ../Location.h ../Options.h \
 ../checkfuncs.h ../keywords.h ../Planet.h ProjectionBase.h ../util.h \
 ../libannotate/libannotate.h getColor.h

ProjectionHemisphere.o: ProjectionHemisphere.cc ../Options.h \
 ../checkfuncs.h ../keywords.h ProjectionHemisphere.h ProjectionBase.h \
 ../util.h

ProjectionMercator.o: ProjectionMercator.cc ../Options.h \
 ../checkfuncs.h ../keywords.h ProjectionMercator.h ProjectionBase.h \
 ../util.h

ProjectionMollweide.o: ProjectionMollweide.cc ../Options.h \
 ../checkfuncs.h ../keywords.h ProjectionMollweide.h ProjectionBase.h \
 ../util.h

ProjectionOrthographic.o: ProjectionOrthographic.cc ../Location.h \
 ../Options.h ../checkfuncs.h ../keywords.h ../Planet.h \
 ProjectionRectangular.h ProjectionBase.h ProjectionOrthographic.h \
 ../Ring.h ../satrings.h ../util.h ../xplanet.h \
 ../libannotate/libannotate.h getColor.h

ProjectionPeters.o: ProjectionPeters.cc ../Options.h ../checkfuncs.h \
 ../keywords.h ProjectionPeters.h ProjectionBase.h ../util.h

ProjectionRectangular.o: ProjectionRectangular.cc ../Location.h \
 ../Options.h ../checkfuncs.h ../keywords.h ProjectionRectangular.h \
 ProjectionBase.h ../Ring.h ../util.h

.PHONY : clean distclean
clean:
	-/bin/rm -f *.o libprojection.a

distclean:
	-/bin/rm -f *.o libprojection.a Makefile
