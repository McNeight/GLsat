/****************************************************************************
    Xplanet 0.94 - render an image of a planet into an X window
    Copyright (C) 2002 Hari Nair <hari@alumni.caltech.edu>

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

#include "getopt.h"

/*
  This is just a wrapper around getopt_long_only, as you can see.
  The Sun and SGI C++ compilers don't like the GNU getopt header file
  and this is a hack around it.
*/

int
xp_getopt_long_only(int __argc, char *const *__argv,
                    const char *__shortopts,
                    const struct option *__longopts, int *__longind)
{
    return(getopt_long_only(__argc, __argv, __shortopts, __longopts, 
                            __longind));
}
