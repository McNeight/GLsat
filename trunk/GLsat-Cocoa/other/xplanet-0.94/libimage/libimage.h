/****************************************************************************
    imagelib.h - header for a simple image manipulation library.
    Distributed with Xplanet.
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

#ifndef IMAGELIB_H
#define IMAGELIB_H

#ifdef __cplusplus
extern "C" {
#endif
    
extern int 
crop_image(unsigned char **rgb, int in_width, int in_height, int x, int y, 
           int width, int height);

extern int
delete_image(unsigned char **rgb);

extern int
read_image(const char *filename, int *width, int *height,
	   unsigned char **rgb);

extern int
resize_image(unsigned char **rgb, int in_width, int in_height, 
             int out_width, int out_height, int bilinear);

extern void
set_alpha(unsigned char *A);

extern void
set_quality(int q);

extern int
write_image(const char *filename, int width, int height, unsigned char *rgb);

#ifdef __cplusplus
}
#endif

#endif 
