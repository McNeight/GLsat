/* checkfuncs.h.  Generated automatically by configure.  */
/****************************************************************************
    Xplanet @version@ - render an image of a planet into an X window
    Copyright (C) 2001 Hari Nair <hari@alumni.caltech.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#ifndef CHECKFUNCS_H
#define CHECKFUNCS_H

#define HAVE_ANIMATION_LIB 1
#define HAVE_AQUA 1
/* #undef HAVE_CYGWIN */
#define HAVE_DREM 1
#define HAVE_LIBFREETYPE 1
#define HAVE_LIBX11 
/* #undef HAVE_XSS */
#define HAVE_STRPTIME 1
/* #undef HAVE_UNSETENV */

#ifdef HAVE_STRPTIME
#define _GNU_SOURCE
#include <time.h>
#endif

#endif
