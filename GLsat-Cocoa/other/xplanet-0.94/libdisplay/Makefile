# Generated automatically from Makefile.in by configure.
prefix = /usr/local

CXX = c++
CPPFLAGS += -Wall -O3 -INONE/include -I${prefix}/include -I..
CXXFLAGS += -I/usr/X11R6/include -I/usr/X11R6/include/freetype2  -I/usr/X11R6/include

SRCS = \
	DisplayBase.cc \
	DisplayMacAqua.cc \
	 \
	DisplayOutput.cc \
	DisplayX11Base.cc DisplayX11.cc \
	getDisplay.cc

OBJS = $(SRCS:.cc=.o)

libdisplay.a: $(OBJS)
	/usr/bin/ar ruv $@ $(OBJS)
	/usr/bin/ranlib $@

DisplayBase.o: DisplayBase.cc ../auxfiles.h ../checkfuncs.h \
 DisplayBase.h ../Location.h ../Marker.h ../Options.h ../keywords.h \
 ../Planet.h ../util.h ../libannotate/libannotate.h \
 ../libprojection/libprojection.h ../libprojection/ProjectionBase.h \
 ../libprojection/ProjectionRectangular.h

DisplayMSWin.o: DisplayMSWin.cc DisplayMSWin.h DisplayBase.h \
 ../checkfuncs.h ../Options.h ../keywords.h \
 ../libannotate/libannotate.h ../libimage/libimage.h \
 ../libprojection/libprojection.h ../libprojection/ProjectionBase.h \
 ../libprojection/ProjectionRectangular.h ../Location.h

DisplayMacAqua.o: DisplayMacAqua.cc DisplayMacAqua.h DisplayBase.h \
 ../checkfuncs.h ../Options.h ../keywords.h \
 ../libannotate/libannotate.h ../libimage/libimage.h \
 ../libprojection/libprojection.h ../libprojection/ProjectionBase.h \
 ../libprojection/ProjectionRectangular.h ../Location.h

DisplayOutput.o: DisplayOutput.cc DisplayOutput.h DisplayBase.h \
 ../checkfuncs.h ../Options.h ../keywords.h ../libimage/libimage.h \
 ../libprojection/libprojection.h ../libprojection/ProjectionBase.h \
 ../libprojection/ProjectionRectangular.h ../Location.h

DisplayX11.o: DisplayX11.cc ../auxfiles.h DisplayX11.h \
 DisplayX11Base.h DisplayBase.h ../checkfuncs.h ../Marker.h \
 ../Options.h ../keywords.h ../util.h ../libannotate/libannotate.h \
 ../libimage/libimage.h

DisplayX11Base.o: DisplayX11Base.cc ../auxfiles.h DisplayX11Base.h \
 DisplayBase.h ../checkfuncs.h ../Marker.h ../Options.h ../keywords.h \
 ../util.h ../libimage/libimage.h ../libprojection/libprojection.h \
 ../libprojection/ProjectionBase.h \
 ../libprojection/ProjectionRectangular.h ../Location.h vroot.h

getDisplay.o: getDisplay.cc ../checkfuncs.h ../keywords.h \
 DisplayBase.h DisplayOutput.h DisplayX11Base.h DisplayX11.h

.PHONY : clean distclean
clean:
	-/bin/rm -f *.o libdisplay.a

distclean:
	-/bin/rm -f *.o libdisplay.a Makefile
