# Generated automatically from Makefile.in by configure.
SHELL = /bin/sh

CP = /bin/cp
GZIP = /sw/bin/gzip
INSTALL = /usr/bin/install -c
RM = /bin/rm
STRIP = /usr/bin/strip
TAR = /sw/bin/tar


prefix = /usr/local
bindir = ${exec_prefix}/bin
exec_prefix = ${prefix}
mandir = ${prefix}/man
auxdir = ${prefix}/share/xplanet
version = 0.94
distdir = xplanet-$(version)

CC = gcc
CXX = c++
CPPFLAGS += -Wall -O3 -INONE/include -I${prefix}/include -I.
CXXFLAGS += -I/usr/X11R6/include -I/usr/X11R6/include/freetype2  -I/usr/X11R6/include

SRCS = xplanet.cc	\
	glutfuncs.cc	\
	Location.cc	\
	Marker.cc	\
	moonpos.cc	\
	Options.cc	\
	Planet.cc	\
	Ring.cc		\
	Satellite.cc	\
	specularMap.cc  \
	ssec.cc		\
	util.cc 

COBJS = getopt.o getopt1.o xp_getopt.o 
OBJS = $(SRCS:.cc=.o) $(COBJS) 
XPLIBS = libdisplay/libdisplay.a libimage/libimage.a 		\
	libprojection/libprojection.a libsgp4sdp4/libsgp4sdp4.a \
	libannotate/libannotate.a 

LIBDIRS = libannotate libdisplay libimage libprojection libsgp4sdp4

all: xplanet xplanetbg

xplanet: $(OBJS) $(XPLIBS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)  -L/usr/X11R6/lib -lX11 \
-framework GLUT -framework OpenGL -lobjc  -L/usr/X11R6/lib -lfreetype \
 -framework ApplicationServices

xplanetbg: xplanetbg.o orbit.o $(COBJS)  libimage/libimage.a
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)   -L/usr/X11R6/lib -lX11  -lm

libannotate/libannotate.a:
	$(MAKE) -C libannotate

libdisplay/libdisplay.a:
	$(MAKE) -C libdisplay

libimage/libimage.a:
	$(MAKE) -C libimage

libprojection/libprojection.a:
	$(MAKE) -C libprojection

libsgp4sdp4/libsgp4sdp4.a:
	$(MAKE) -C libsgp4sdp4

xplanet.o: xplanet.cc auxfiles.h checkfuncs.h Options.h keywords.h \
 Planet.h util.h libannotate/libannotate.h libdisplay/libdisplay.h \
 libdisplay/DisplayBase.h libimage/libimage.h \
 libprojection/libprojection.h libprojection/ProjectionBase.h \
 libprojection/ProjectionRectangular.h Location.h

glutfuncs.o: glutfuncs.cc checkfuncs.h auxfiles.h xplanet.h \
 libprojection/libprojection.h libprojection/ProjectionBase.h \
 libprojection/ProjectionRectangular.h Location.h

Location.o: Location.cc Location.h util.h

Marker.o: Marker.cc Marker.h Options.h checkfuncs.h keywords.h util.h

moonpos.o: moonpos.cc util.h

Options.o: Options.cc checkfuncs.h auxfiles.h Options.h keywords.h \
 util.h xp_getopt.h libannotate/libannotate.h \
 libprojection/libprojection.h libprojection/ProjectionBase.h \
 libprojection/ProjectionRectangular.h Location.h

Planet.o: Planet.cc Options.h checkfuncs.h keywords.h Planet.h util.h

Ring.o: Ring.cc Location.h Options.h checkfuncs.h keywords.h Ring.h \
 satrings.h util.h

Satellite.o: Satellite.cc Options.h checkfuncs.h keywords.h util.h \
 Satellite.h libsgp4sdp4/sgp4sdp4.h

ssec.o: ssec.cc Options.h checkfuncs.h keywords.h util.h \
 libimage/libimage.h

util.o: util.cc auxfiles.h Options.h checkfuncs.h keywords.h util.h \
 xplanet.h libimage/libimage.h libprojection/libprojection.h \
 libprojection/ProjectionBase.h libprojection/ProjectionRectangular.h \
 Location.h

xplanetbg.o: xplanetbg.cc checkfuncs.h orbit.h xp_getopt.h xplanetbg.h \
 libimage/libimage.h

orbit.o: orbit.cc orbit.h

install: all
	$(INSTALL) -d $(bindir)
	$(INSTALL) xplanet $(bindir)
	$(INSTALL) xplanetbg $(bindir)
	$(INSTALL) -m 755 tkxplanet $(bindir)
	$(INSTALL) -m 755 tzcoord.pl $(bindir)
	$(INSTALL) -d $(mandir)
	$(INSTALL) -d $(mandir)/man1
	$(INSTALL) -m 644 tkxplanet.1 $(mandir)/man1
	$(INSTALL) -m 644 tzcoord.pl.1 $(mandir)/man1
	$(INSTALL) -m 644 xplanet.1 $(mandir)/man1
	$(INSTALL) -m 644 xplanetbg.1 $(mandir)/man1
	$(INSTALL) -d $(auxdir)
	-$(CP) -r auxdir/* $(auxdir)

install-strip: install
	$(STRIP) $(bindir)/xplanet
	$(STRIP) $(bindir)/xplanetbg

uninstall:
	-$(RM) -f $(bindir)/xplanet
	-$(RM) -f $(bindir)/xplanetbg
	-$(RM) -f $(bindir)/tkxplanet
	-$(RM) -f $(bindir)/tzcoord.pl
	-$(RM) -f $(mandir)/man1/xplanet.1
	-$(RM) -f $(mandir)/man1/xplanetbg.1
	@echo "*** Not removing " $(auxdir) " ***"

dist: 	
	$(INSTALL) -d $(distdir)
	$(CP) COPYING CREDITS ChangeLog FAQ INSTALL Makefile* README *.c \
	*.cc *.h *.h.in tkxplanet* tzcoord.pl* aclocal.m4 configure* *.1 \
	$(distdir)
	$(CP) -r autoconf $(distdir)
	list='$(LIBDIRS)'; for subdir in $$list; do \
	($(CP) -r $$subdir $(distdir)) \
	done
	$(CP) -r auxdir $(distdir)
	$(TAR) cf $(distdir).tar $(distdir)
	$(GZIP) -9 $(distdir).tar
	-$(RM) -fr $(distdir)

.PHONY : all clean distclean
clean:
	-$(RM) -f *.o
	list='$(LIBDIRS)'; for subdir in $$list; do	\
	(cd $$subdir; $(MAKE) clean)			\
	done

distclean:
	-$(RM) -f *.o auxfiles.h checkfuncs.h xplanet	\
	xplanetbg xplanetbg.h					\
	config.* tkxplanet tzcoord.pl Makefile;			\
	list='$(LIBDIRS)'; for subdir in $$list; do		\
	(cd $$subdir; $(MAKE) distclean)			\
	done
