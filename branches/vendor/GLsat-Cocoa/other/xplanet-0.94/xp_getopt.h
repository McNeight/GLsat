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

#ifdef __cplusplus
extern "C" {
#endif

    extern char *optarg;
    extern int optind;

    struct option
    {
# if defined __STDC__ && __STDC__
        const char *name;
# else
        char *name;
# endif
        /* has_arg can't be an enum because some compilers complain about
           type mismatches in all the code that assumes it is an int.  */
        int has_arg;
        int *flag;
        int val;
    };

# define no_argument            0
# define required_argument      1
# define optional_argument      2

    extern int xp_getopt_long_only(int __argc, char *const *__argv,
                                   const char *__shortopts,
                                   const struct option *__longopts, 
                                   int *__longind);

#ifdef __cplusplus
}
#endif
