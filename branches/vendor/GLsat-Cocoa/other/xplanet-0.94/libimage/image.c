/****************************************************************************
    image.c - a simple image manipulation library.  Distributed with Xplanet.
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "checkfuncs.h"

int
read_bmp(const char *filename, int *width, int *height, unsigned char **rgb);
int
write_bmp(const char *filename, int width, int height, unsigned char *rgb);

#ifdef HAVE_LIBGIF
int
read_gif(const char *filename, int *width, int *height, unsigned char **rgb);
int
write_gif(const char *filename, int width, int height, unsigned char *rgb);
#endif

#ifdef HAVE_LIBJPEG
int
read_jpeg(const char *filename, int *width, int *height, unsigned char **rgb);
int
write_jpeg(FILE *outfile, int width, int height, unsigned char *rgb, 
           int quality);
#endif

#ifdef HAVE_LIBPNG
int
read_png(const char *filename, int *width, int *height, unsigned char **rgb);
int
write_png(FILE *outfile, int width, int height, unsigned char *rgb, 
	  unsigned char *alpha);
#endif

#ifdef HAVE_LIBPNM
#include <pnm.h>
int
read_pnm(const char *filename, int *width, int *height, unsigned char **rgb);
int
write_pnm(FILE *outfile, int width, int height, unsigned char *rgb,
          int maxv, int format, int forceplain);
#endif

#ifdef HAVE_LIBTIFF
int
read_tiff(const char *filename, int *width, int *height, unsigned char **rgb);
int
write_tiff(const char *filename, int width, int height, unsigned char *rgb);
#endif

static unsigned char *alpha;     /* PNG alpha (opacity) channel */
static int Q;                    /* JPEG Quality */

void
set_alpha(unsigned char *A)
{
    alpha = A;
}

void
set_quality(int q)
{
    Q = q;
}

int
read_image(const char *filename, int *width, int *height, 
	   unsigned char **rgb)
{
    char buf[4];
    unsigned char *ubuf = (unsigned char *) buf;
    int success = 0;

    FILE *file;
    file = fopen(filename, "rb");
    if (file == NULL) return(0);
  
    /* see what kind of file we have */

    fread(buf, 1, 4, file);
    fclose(file);

    if (!strncmp("BM", buf, 2))
    {
        success = read_bmp(filename, width, height, rgb);
    }
    else if (!strncmp("GIF8", buf, 4))
    {
#ifdef HAVE_LIBGIF
        success = read_gif(filename, width, height, rgb);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with GIF support\n");
        success = 0;
#endif /* HAVE_LIBGIF */
    }
    else if ((ubuf[0] == 0xff) && (ubuf[1] == 0xd8))
    {
#ifdef HAVE_LIBJPEG
        success = read_jpeg(filename, width, height, rgb);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with JPEG support\n");
        success = 0;
#endif /* HAVE_LIBJPEG */
    }
    else if ((ubuf[0] == 0x89) && !strncmp("PNG", buf+1, 3))
    {
#ifdef HAVE_LIBPNG
        success = read_png(filename, width, height, rgb);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with PNG support\n");
        success = 0;
#endif /* HAVE_LIBPNG */
    }
    else if ((   !strncmp("P6\n", buf, 3))
             || (!strncmp("P5\n", buf, 3))
             || (!strncmp("P4\n", buf, 3))
             || (!strncmp("P3\n", buf, 3))
             || (!strncmp("P2\n", buf, 3))
             || (!strncmp("P1\n", buf, 3)))
    {
#ifdef HAVE_LIBPNM
        success = read_pnm(filename, width, height, rgb);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with PNM support\n");
        success = 0;
#endif /* HAVE_LIBPNM */
    }
    else if (((!strncmp ("MM", buf, 2)) && (ubuf[2] == 0x00) 
              && (ubuf[3] == 0x2a))
             || ((!strncmp ("II", buf, 2)) && (ubuf[2] == 0x2a) 
                 && (ubuf[3] == 0x00)))
    {
#ifdef HAVE_LIBTIFF
        success = read_tiff(filename, width, height, rgb);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with TIFF support\n");
        success = 0;
#endif
    }
    else
    {
        fprintf(stderr, "Unknown image format\n");
        success = 0;
    }

    return(success);
}

int
write_image(const char *filename, int width, int height, unsigned char *rgb)
{
    FILE *outfile;
    char *extension = strrchr(filename, '.');
    char *lowercase;
    char *ptr;
    int success = 0;
  
    lowercase = malloc(strlen(extension) + 1);
    strcpy(lowercase, extension);
    ptr = lowercase;

    while (*ptr != '\0') *ptr++ = tolower(*extension++);

    outfile = fopen(filename, "wb");
    if (outfile == NULL) return(0);
  
    if (strcmp(lowercase, ".bmp" ) == 0)
    {
        success = write_bmp(filename, width, height, rgb); 
    }
    else if (strcmp(lowercase, ".gif" ) == 0)
    {
#ifdef HAVE_LIBGIF
        success = write_gif(filename, width, height, rgb); 
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with GIF support\n");
        success = 0;
#endif /* HAVE_LIBPNG */
    }
    else if ((   strcmp(lowercase, ".jpg" ) == 0)
             || (strcmp(lowercase, ".jpeg") == 0))
    {
#ifdef HAVE_LIBJPEG
        success = write_jpeg(outfile, width, height, rgb, Q); 
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with JPEG support\n");
        success = 0;
#endif /* HAVE_LIBJPEG */
    }

    else if (strcmp(lowercase, ".png" ) == 0)
    {
#ifdef HAVE_LIBPNG
        success = write_png(outfile, width, height, rgb, alpha); 
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with PNG support\n");
        success = 0;
#endif /* HAVE_LIBPNG */
    }

    else if ((   strcmp(lowercase, ".pbm") == 0)
             || (strcmp(lowercase, ".pgm") == 0)
             || (strcmp(lowercase, ".ppm") == 0))
    {
#ifdef HAVE_LIBPNM
        if (strcmp(lowercase, ".pbm") == 0)
            success = write_pnm(outfile, width, height, rgb, 1, PBM_TYPE, 0);
        else if (strcmp(lowercase, ".pgm") == 0)
            success = write_pnm(outfile, width, height, rgb, 255, 
                                PGM_TYPE, 0);
        else if (strcmp(lowercase, ".ppm") == 0)
            success = write_pnm(outfile, width, height, rgb, 255, 
                                PPM_TYPE, 0);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with PNM support\n");
        success = 0;
#endif /* HAVE_LIBPNM */
    }

    else if ((strcmp(lowercase, ".tif" ) == 0)
             || (strcmp(lowercase, ".tiff" ) == 0))
    {
#ifdef HAVE_LIBTIFF
        success = write_tiff(filename, width, height, rgb); 
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with TIFF support\n");
        success = 0;
#endif /* HAVE_LIBTIFF */
    }

    else
    {
        fprintf(stderr, "Unknown image format\n");
        success = 0;
    }

    free(lowercase);
    fclose(outfile);
    return(success);
}

int
delete_image(unsigned char **rgb)
{
    free(rgb[0]);
    rgb[0] = NULL;
    return(1);
}

int 
crop_image(unsigned char **rgb, int in_width, int in_height, int x, int y, 
           int width, int height)
{
    int i, j;
    int ipos = 0;
    unsigned char *old_rgb = rgb[0];
    unsigned char *new_rgb = malloc(3 * width * height);

    if (new_rgb == NULL)
    {
        fprintf(stderr, "Can't allocate memory in crop_image().\n");
        return(0);
    }
    
    for (j = 0; j < height; j++)
        for (i = 0; i < width; i++)
        {
            memcpy(new_rgb + ipos, 
                   old_rgb + 3 * ((j + y) * in_width + (i + x)), 3);
            ipos += 3;
        }
    rgb[0] = new_rgb;
    free(old_rgb);
    return(1);
}

int
resize_image(unsigned char **rgb, int in_width, int in_height, 
             int out_width, int out_height, int bilinear)
{
    int i, j, ii, jj;
    double dx, dy;
    double t, u;
    int in_pos[4], out_pos;
    int sum;
    double weight[4];
    int ix[4], iy[4];

    double frac_h = ((double) in_height - 1) / (out_height - 1);
    double frac_w = ((double) in_width - 1) / (out_width - 1);

    unsigned char *old_rgb = rgb[0];
    unsigned char *new_rgb = malloc(3 * out_width * out_height);

    if (new_rgb == NULL)
    {
        fprintf(stderr, "Can't allocate memory in resize_image().\n");
        return(0);
    }

    if (in_width == out_width && in_height == out_height) return(1);

    if (bilinear)
    {
        for (j = 0; j < out_height; j++)
        {
            dy = ((double) j / (out_height - 1)) * (in_height - 1);
          
            iy[0] = iy[1] = (int) dy;
            iy[2] = iy[0] + 1;
            if (iy[2] == in_height) iy[2]--;
            iy[3] = iy[2];
          
            u = 1 - (dy - iy[0]);
            for (i = 0; i < out_width; i++)
            {
                dx = ((double) i / (out_width - 1)) * (in_width - 1);
              
                ix[0] = ix[2] = (int) dx;
                ix[1] = ix[0] + 1;
                if (ix[1] == in_width) ix[1] = 0;
                ix[3] = ix[1];

                t = dx - ix[0];
                if (t > in_width/2) 
                    t -= in_width;
                else if (t < -in_width/2) 
                    t += in_width;
              
                /*
                  Weights are from Numerical Recipes, 2nd Edition
                  weight[0] = (1 - t) * u;
                  weight[1] = t * u;
                  weight[2] = (1-t) * (1-u);
                  weight[3] = t * (1-u);
                */
              
                weight[1] = t * u;
                weight[0] = u - weight[1];
                weight[2] = 1 - t - u + weight[1];
                weight[3] = t - weight[1];
          
                for (jj = 0; jj < 4; jj++)
                    in_pos[jj] = 3 * (iy[jj] * in_width + ix[jj]);

                out_pos = 3 * (j * out_width + i);
                for (ii = 0; ii < 3; ii++)
                {
                    sum = 0;
                    for (jj = 0; jj < 4; jj++)
                        sum += (int) (weight[jj] * rgb[0][in_pos[jj] + ii]);
                    new_rgb[out_pos + ii] = (unsigned char) (sum & 0xff);
                }
            }
        }
    }
    else
    {
        out_pos = 0;
        for (j = 0; j < out_height; j++)
        {
            dy = j * frac_h;
            iy[0] = (int) dy;
            for (i = 0; i < out_width; i++)
            {
                dx = i * frac_w;
                ix[0] = (int) dx;
                in_pos[0] = 3 * (iy[0] * in_width + ix[0]);

                memcpy(new_rgb + out_pos, rgb[0] + in_pos[0], 3);
                out_pos += 3;
            }
        }
    }

    rgb[0] = new_rgb;
    free(old_rgb);
    return(1);
}
