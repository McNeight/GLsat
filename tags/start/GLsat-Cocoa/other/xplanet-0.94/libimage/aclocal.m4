AC_DEFUN(AC_FIND_FREETYPE,
[

AC_ARG_WITH(freetype, [  --with-freetype         Enable Freetype support for TrueType fonts [yes]])

if test "$with_freetype" != 'no'; then
  AC_PATH_X_PROG(FREETYPE_CONFIG, freetype-config, no)
  if test "$FREETYPE_CONFIG" = no; then
    AC_MSG_WARN(*** Xplanet will be built without freetype support ***)
    with_freetype='no'
  else
    FREETYPE_CFLAGS=`$FREETYPE_CONFIG --cflags`
    FREETYPE_LIBS=`$FREETYPE_CONFIG --libs`
    AC_SUBST(FREETYPE_CFLAGS)
    AC_SUBST(FREETYPE_LIBS)
    AC_DEFINE(HAVE_LIBFREETYPE)
  fi
fi

])

dnl Autoconf stuff to check for graphics libraries is adapted from 
dnl imagemagick's configure.in

AC_DEFUN(AC_FIND_GRAPHICS_LIBS,

[

GRAPHICS_SOURCE=""
GRAPHICS_LIBS=""

#
# Check for GIF
#
AC_ARG_WITH(gif, [  --with-gif              Enable GIF support [yes]])

have_gif='no'
if test "$with_gif" != 'no'; then
    have_gif_header='no'
    AC_CHECK_X_HEADER(gif_lib.h,have_gif_header='yes',have_gif_header='no')
    if test "$have_gif_header" != 'no'; then
        have_libgif='no'
        AC_CHECK_X_LIB(gif, DGifOpenFileName,have_libgif='yes',have_libgif='no',$X_LIBS)

	if test "$have_libgif" != 'no'; then
	    GRAPHICS_SOURCE="$GRAPHICS_SOURCE gif.c"
            GRAPHICS_LIBS="$GRAPHICS_LIBS -lgif $X_LIBS"
	    AC_DEFINE(HAVE_LIBGIF,,Define if you have GIF library)
	    have_gif='yes'
	else
            AC_CHECK_X_LIB(ungif, DGifOpenFileName,have_libgif='yes',have_libgif='no',$X_LIBS)
            if test "$have_libgif" != 'no'; then
	        GRAPHICS_SOURCE="$GRAPHICS_SOURCE gif.c"
                GRAPHICS_LIBS="$GRAPHICS_LIBS -lungif"
                AC_DEFINE(HAVE_LIBGIF,,Define if you have GIF library)
	        have_gif='yes'
	    fi
	fi

    fi
    if test "$have_gif" != 'yes'; then
	AC_MSG_WARN(*** Native GIF support will not be included ***)
    fi
fi

#
# Check for JPEG
#
AC_ARG_WITH(jpeg, [  --with-jpeg             Enable JPEG support [yes]])

have_jpeg='no'
if test "$with_jpeg" != 'no'; then
    have_jpeg_header='no'
    AC_CHECK_X_HEADER(jpeglib.h,have_jpeg_header='yes',have_jpeg_header='no')
    if test "$have_jpeg_header" != 'no'; then
	have_libjpeg='no'
        AC_CHECK_X_LIB(jpeg,jpeg_read_header,have_libjpeg='yes',have_libjpeg='no',)
	if test "$have_libjpeg" != 'no'; then
	    GRAPHICS_SOURCE="$GRAPHICS_SOURCE jpeg.c"
	    GRAPHICS_LIBS="$GRAPHICS_LIBS -ljpeg"
	    AC_DEFINE(HAVE_LIBJPEG,,Define if you have JPEG library)
            have_jpeg='yes'
  	fi
    fi
    if test "$have_jpeg" != 'yes'; then
	AC_MSG_WARN(*** Native JPEG support will not be included ***)
    fi
fi

#
# Check for PNG
#
AC_ARG_WITH(png, [  --with-png              Enable PNG support [yes]])

have_png='no'
if test "$with_png" != 'no'; then
    have_png_header='no'
    AC_CHECK_X_HEADER(png.h,have_png_header='yes',have_png_header='no')
    AC_CHECK_X_HEADER(pngconf.h,have_png_header='yes',have_png_header='no')
    if test "$have_png_header" != 'no'; then
	have_libpng='no'
	AC_CHECK_X_LIB(png,png_create_read_struct,have_libpng='yes',have_libpng='no',-lm -lz)
	if test "$have_libpng" != 'no'; then
	    GRAPHICS_SOURCE="$GRAPHICS_SOURCE png.c"
	    GRAPHICS_LIBS="$GRAPHICS_LIBS -lpng -lm -lz"
	    AC_DEFINE(HAVE_LIBPNG,,Define if you have PNG library)
            have_png='yes'
  	fi
    fi
    if test "$have_png" != 'yes'; then
	AC_MSG_WARN(*** Native PNG support will not be included ***)
    fi
fi

#
# Check for PNM
#
AC_ARG_WITH(pnm, [  --with-pnm              Enable PNM support [yes]])

have_pnm='no'
if test "$with_pnm" != 'no'; then
    have_pnm_header='no'
    AC_CHECK_X_HEADER(pnm.h,have_pnm_header='yes',have_pnm_header='no')
    if test "$have_pnm_header" != 'no'; then
	have_libpnm='no'
        AC_CHECK_X_LIB(netpbm,pnm_init,have_libpnm='yes',have_libpnm='no',)
	if test "$have_libpnm" != 'no'; then
	    GRAPHICS_SOURCE="$GRAPHICS_SOURCE pnm.c"
	    GRAPHICS_LIBS="$GRAPHICS_LIBS -lnetpbm"
	    AC_DEFINE(HAVE_LIBPNM,,Define if you have PNM library)
            have_pnm='yes'
	else
            AC_CHECK_X_LIB(pnm,pnm_init,have_libpnm='yes',have_libpnm='no',)
	    if test "$have_libpnm" != 'no'; then
	        GRAPHICS_SOURCE="$GRAPHICS_SOURCE pnm.c"
	        GRAPHICS_LIBS="$GRAPHICS_LIBS -lpnm"
	        AC_DEFINE(HAVE_LIBPNM,,Define if you have PNM library)
                have_pnm='yes'
	    fi
  	fi
    fi
    if test "$have_pnm" != 'yes'; then
	AC_MSG_WARN(*** Native PNM support will not be included ***)
    fi
fi

#
# Check for TIFF
#
AC_ARG_WITH(tiff, [  --with-tiff             Enable TIFF support [yes]])

have_tiff='no'
if test "$with_tiff" != 'no'; then
    have_tiff_header='no'
    AC_CHECK_X_HEADER(tiff.h,have_tiff_header='yes',have_tiff_header='no')
    AC_CHECK_X_HEADER(tiffio.h,have_tiff_header='yes',have_tiff_header='no')
    if test "$have_tiff_header" != 'no'; then
	have_libtiff='no'
        AC_CHECK_X_LIB(tiff,TIFFOpen,have_libtiff='yes',have_libtiff='no',-lm)
	if test "$have_libtiff" != 'no'; then
	    GRAPHICS_SOURCE="$GRAPHICS_SOURCE tiff.c"
	    GRAPHICS_LIBS="$GRAPHICS_LIBS -ltiff -lm"
	    AC_DEFINE(HAVE_LIBTIFF,,Define if you have TIFF library)
            have_tiff='yes'
  	fi
    fi
    if test "$have_tiff" != 'yes'; then
	AC_MSG_WARN(*** Native TIFF support will not be included ***)
    fi
fi

AC_SUBST(GRAPHICS_SOURCE)
AC_SUBST(GRAPHICS_LIBS)

])

# Macros to search for OpenGL/Mesa are adapted from the
# xscreensaver-3.14 package

###############################################################################
#
#       Some utility functions to make checking for X things easier.
#
###############################################################################

# Like AC_CHECK_HEADER, but it uses the already-computed -I directories.
#
AC_DEFUN(AC_CHECK_X_HEADER, [
  ac_save_CPPFLAGS="$CPPFLAGS"
  if test \! -z "$prefix" ; then 
    CPPFLAGS="$CPPFLAGS -I$prefix/include"
  fi
  if test \! -z "$includedir" ; then 
    CPPFLAGS="$CPPFLAGS -I$includedir"
  fi
  CPPFLAGS="$CPPFLAGS $X_CFLAGS"
  AC_CHECK_HEADER([$1], [$2], [$3])
  CPPFLAGS="$ac_save_CPPFLAGS"])

AC_DEFUN(AC_CHECK_X_LIB, [
  ac_save_LDFLAGS="$LDFLAGS"
  if test "$prefix" != "NONE"; then 
    LDFLAGS="$LDFLAGS -L$prefix/lib"
  fi
  if test \! -z "$libdir" ; then 
    LDFLAGS="$LDFLAGS -L$libdir"
  fi
  AC_CHECK_LIB([$1], [$2], [$3], [$4], [$5])
  LDFLAGS="$ac_save_LDFLAGS"
]
)

# Like AC_EGREP_HEADER, but it uses the already-computed -I directories.
#
AC_DEFUN(AC_EGREP_X_HEADER, [
  ac_save_CPPFLAGS="$CPPFLAGS"
  if test \! -z "$prefix" ; then 
    CPPFLAGS="$CPPFLAGS -I$prefix/include"
  fi
  if test \! -z "$includedir" ; then 
    CPPFLAGS="$CPPFLAGS -I$includedir"
  fi
  CPPFLAGS="$CPPFLAGS $X_CFLAGS"
  AC_EGREP_HEADER([$1], [$2], [$3], [$4])
  CPPFLAGS="$ac_save_CPPFLAGS"])

# Usage: HANDLE_X_PATH_ARG([variable_name],
#                          [--command-line-option],
#                          [descriptive string])
#
# All of the --with options take three forms:
#
#   --with-foo (or --with-foo=yes)
#   --without-foo (or --with-foo=no)
#   --with-foo=/DIR
#
# This function, HANDLE_X_PATH_ARG, deals with the /DIR case.  When it sees
# a directory (string beginning with a slash) it checks to see whether
# /DIR/include and /DIR/lib exist, and adds them to $X_CFLAGS and $X_LIBS
# as appropriate.
#
AC_DEFUN(HANDLE_X_PATH_ARG, [

   case "$[$1]" in
    yes) ;;
    no)  ;;

    /*)
     AC_MSG_CHECKING([for existence of [$3] headers directory])
     d=$[$1]/include
     if test -d $d; then
       X_CFLAGS="-I$d $X_CFLAGS"
       AC_MSG_RESULT($d)
     else
       AC_MSG_RESULT(not found ($d: no such directory))
     fi

     AC_MSG_CHECKING([for existence of [$3] libs directory])
     d=$[$1]/lib
     if test -d $d; then
       X_LIBS="-L$d $X_LIBS"
       AC_MSG_RESULT($d)
     else
       AC_MSG_RESULT(not found ($d: no such directory))
     fi

     # replace the directory string with "yes".
     [$1]_req="yes"
     [$1]=$[$1]_req
     ;;

    *)
     echo ""
     echo "error: argument to [$2] must be \"yes\", \"no\", or a directory."
     echo "       If it is a directory, then \`DIR/include' will be added to"
     echo "       the -I list, and \`DIR/lib' will be added to the -L list."
     exit 1
     ;;
   esac
  ])

AC_DEFUN(AC_PATH_X_PROG, [
  ac_save_PATH="$PATH"
  if test \! -z "$prefix" ; then 
    PATH="$PATH:${prefix}/bin"
  fi
  if test \! -z "$bindir" ; then 
    PATH="$PATH:${bindir}"
  fi
  AC_PATH_PROG([$1], [$2], [$3])
  PATH="$ac_save_PATH"
]
)

#
# Check for X11
#

AC_DEFUN(AC_FIND_X11,
[

have_libx11='no'
if test "$with_x" != 'no'; then

dnl Locate X include files and libraries
   AC_PATH_XTRA
   NEW_X_LIBS="$X_LIBS -lX11"

   AC_CHECK_LIB(X11, XOpenDisplay, have_libx11='yes',have_libx11='no',$X_LIBS)
   if test "$have_libx11" != 'no'; then
     AC_DEFINE(HAVE_LIBX11,,Define if you have X11 libraries)
     DISPLAYX11="DisplayX11Base.cc DisplayX11.cc"

     AC_SUBST(DISPLAYX11)
     X_LIBS="$NEW_X_LIBS"
   else
     AC_MSG_WARN(*** Xplanet will be built without X11 support ***)
   fi
fi

if test "$have_libx11" = 'no'; then
  XPARSEGEOMETRY="ParseGeom.o"
  AC_SUBST(XPARSEGEOMETRY)
fi

])

AC_DEFUN(AC_FIND_MAGICK,
[

AC_ARG_WITH(magick, [  --with-magick           Use ImageMagick libraries [no]],use_magick=yes,use_magick=no)

if test "$use_magick" = yes; then
  AC_PATH_X_PROG(MAGICK_CONFIG, Magick++-config, no)
  if test "$MAGICK_CONFIG" = no; then
    AC_MSG_WARN(*** Xplanet will be built without ImageMagick support ***)
    use_magick=no
  else
    AC_MSG_RESULT([ImageMagick version is `$MAGICK_CONFIG --version`])
    MAGICK_CFLAGS=`$MAGICK_CONFIG --cppflags`
    MAGICK_CFLAGS="$MAGICK_CFLAGS `$MAGICK_CONFIG --cxxflags`"
    MAGICK_LIBS=`$MAGICK_CONFIG --ldflags`
    MAGICK_LIBS="$MAGICK_LIBS `$MAGICK_CONFIG --libs`"
    AC_SUBST(MAGICK_CFLAGS)
    AC_SUBST(MAGICK_LIBS)
    AC_DEFINE(HAVE_LIBMAGICK)
    DISPLAYMAGICK="DisplayMagickpp.cc"
    DISPLAYMAGICKHEADER="DisplayMagickpp.h"
    AC_SUBST(DISPLAYMAGICK)
    AC_SUBST(DISPLAYMAGICKHEADER)
  fi
fi

])

AC_DEFUN(AC_USE_MACAQUA,
[
AC_ARG_WITH(aqua, [  --with-aqua             For Mac OS X Aqua [no]],use_aqua=yes,use_aqua=no)

if test "$use_aqua" = yes; then
  DISPLAYMACAQUA="DisplayMacAqua.cc"
  DISPLAYMACAQUAHEADER="DisplayMacAqua.h"

  AC_SUBST(DISPLAYMACAQUA)
  AC_SUBST(DISPLAYMACAQUAHEADER)

  AC_DEFINE(HAVE_AQUA)
  ADDITIONAL_LIBS="$ADDITIONAL_LIBS -framework ApplicationServices"
  AC_SUBST(ADDITIONAL_LIBS)
fi

])

AC_DEFUN(AC_USE_MSWIN,
[
AC_ARG_WITH(mswin, [  --with-mswin            For MS Windows [no]],use_mswin=yes,use_mswin=no)

separator="/"

if test "$use_mswin" = yes; then
dnl  AC_DEFINE(_WIN32,,Define for 32 bit MS Windows)

  DISPLAYMSWIN="DisplayMSWin.cc"
  DISPLAYMSWINHEADER="DisplayMSWin.h"
  separator="\\\\"

  AC_SUBST(DISPLAYMSWIN)
  AC_SUBST(DISPLAYMSWINHEADER)
fi

AC_SUBST(separator)

])

AC_DEFUN(AC_FIND_GL,
[

AC_ARG_WITH(animation, [  --with-animation        Enable --animation option (OpenGL/Mesa 
                          must be installed in this case) [yes]])
if test "$with_animation" != no; then

if test "$use_aqua" = yes; then
  AC_CHECK_X_HEADER(GLUT/glut.h, have_glut='yes', have_glut='no')
  if test "$have_glut" != 'no'; then
    AC_DEFINE(HAVE_ANIMATION_LIB)
    GLUTFUNCS="glutfuncs.cc"
    AC_SUBST(GLUTFUNCS)
    OPENGL_LIBS="-framework GLUT -framework OpenGL -lobjc"
    AC_SUBST(OPENGL_LIBS)
  fi
else

AC_ARG_WITH(gl, [  --with-gl               Assume GL libraries exist [no]])
AC_ARG_WITH(glut, [  --with-glut             Assume GLUT libraries exist [no]])

AC_ARG_WITH(gl_dir,[  --with-gl-dir=DIR       Location of OpenGL/Mesa headers and libraries],
  [with_gl_dir="$withval"; with_gl_dir_req="$withval"],[with_gl_dir=yes])
AC_ARG_WITH(glut_dir,[  --with-glut-dir=DIR     Location of GLUT headers and libraries],
  [with_glut_dir="$withval"; with_glut_dir_req="$withval"],[with_glut_dir=no])

if test "$with_glut_dir" != no; then
  HANDLE_X_PATH_ARG(with_glut_dir, --with-glut-dir, GLUT)
fi

if test "$with_glut" = 'yes'; then
  echo "Not checking for glut; assuming it exists ..."
  have_glut='yes'
else
  have_glut='no'
  have_glut_header='no'
  AC_CHECK_X_HEADER(GL/glut.h, have_glut_header='yes', have_glut_header='no')
  if test "$have_glut_header" != 'no'; then
    have_libglut='no'
    if test "$use_mswin" = yes; then
      have_libglut='yes'
    else
      AC_CHECK_LIB(glut,glutInit,have_libglut='yes',have_libglut='no',$X_LIBS -lGL -lGLU -lXmu -lX11 -lm)
    fi
    if test "$have_libglut" != 'no'; then
      have_glut='yes'
    fi
  fi
fi

###############################################################################
#
#       Check for -lGL or -lMesa.
#
###############################################################################

HANDLE_X_PATH_ARG(with_gl_dir, --with-gl-dir, GL)

if test "$with_gl" = yes; then
  echo "Not checking for OpenGL/Mesa; assuming it exists ..."
  have_gl='yes'
else
  have_gl='no'
  if test "$with_gl_dir" = yes; then
    AC_CHECK_X_HEADER(GL/gl.h, have_gl_header=yes, have_gl_header=no)
    if test "$have_gl_header" != 'no'; then
      have_libgl='no'
      if test "$use_mswin" = yes; then
        have_libgl='yes'
      else
        AC_CHECK_LIB(GL,glBegin,have_libgl='yes',have_libgl='no',$X_LIBS)
      fi
      have_glu_header='no'
      if test "$have_libgl" != 'no'; then
        AC_CHECK_X_HEADER(GL/glu.h, have_glu_header=yes, have_glu_header=no)
        if test "$have_glu_header" != 'no'; then
          have_libglu='no'
          if test "$use_mswin" = yes; then
            have_libglu='yes'
          else
            AC_CHECK_LIB(GLU,gluSphere,have_libglu='yes',have_libglu='no',$X_LIBS -lGL -lm)
          fi
          if test "$have_libglu" != 'no'; then
            have_gl='yes'
          fi
        fi
      fi
    fi
  elif test "$with_gl_dir" != no; then
    echo "error: must be yes or no: --with-gl-dir=$with_gl_dir"
    exit 1
  fi
fi

if test "$have_glut" = yes && test "$have_gl" = yes; then
  AC_DEFINE(HAVE_ANIMATION_LIB)
  GLUTFUNCS="glutfuncs.cc"
  AC_SUBST(GLUTFUNCS)
  if test "$use_mswin" = yes; then
    OPENGL_LIBS="-lglut32 -lopengl32 -lglu32"
  else
    OPENGL_LIBS="-lXmu -lXi -lglut -lGL -lGLU"
  fi
  AC_SUBST(OPENGL_LIBS)
else
    AC_MSG_WARN(*** Xplanet will be built without the -animate option ***)
fi

fi

fi
])
