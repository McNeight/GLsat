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

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

#include "Marker.h"
#include "Options.h"
#include "util.h"

static int num_markers = 0;

Marker::Marker(const int X, const int Y,
               const string Text, const int Align)
{
    ID = num_markers++;

    x = X;
    y = Y;
    text.assign(Text);
    align = Align;

    image_width = -1;
    image_height = -1;
    image_data = NULL;

    color = "";
    font = opts.font;
    fontsize = opts.fontsize;

    transparency = false;

    if (align == AUTO)
    {
        fixed_align = false;
        align = RIGHT;
    }
    else
    {
        fixed_align = true;
    }
}

Marker::~Marker()
{
}

bool
Marker::operator < (const Marker &m)
{
    return(x < m.x);
}

bool
Marker::operator > (const Marker &m)
{
    return(x > m.x);
}

bool
Marker::operator == (const Marker &m)
{
    return(x == m.x);
}

bool
Marker::operator != (const Marker &m)
{
    return(x != m.x);
}

void
Marker::setImage(const int width, const int height)
{
    image_width = width;
    image_height = height;
    image.left = x - width/2;
    image.right = x + width/2;
    image.top = y - height/2;
    image.bottom = y + height/2;
}

void
Marker::setImage(string filename)
{
    if (readImageFile(filename, image_width, image_height, image_data))
    {
        image.left = x - image_width/2;
        image.right = x + image_width/2;
        image.top = y - image_height/2;
        image.bottom = y + image_height/2;
    }
    else
    {
        cerr << "Can't read image " << filename << endl;
        setImage(0, 0);
    }
}

void
Marker::setCorners(const int x1, const int y1, 
                   const int x2, const int y2)
{
    label.left = min(x1, x2);
    label.right = max(x1, x2);
    label.top = min(y1, y2);
    label.bottom = max(y1, y2);
}

int
Marker::overlap(const Marker &m)
{
    return(overlap(m.image) + overlap(m.label));
}

int
Marker::overlap(const BoundingBox &b)
{
    if (label.left > b.right || label.right < b.left) return(0);

    if (label.top > b.bottom || label.bottom < b.top) return(0);

    int width, height;
    if (label.left > b.left)
        width = min(b.right, label.right) - label.left;
    else
        width = min(b.right, label.right) - max(b.left, label.left);

    if (label.top > b.top)
        height = min(b.bottom, label.bottom) - label.top;
    else
        height = min(b.bottom, label.bottom) - max(b.top, label.top);

    return(width * height);
}

void
Marker::printCorners(ofstream &outfile)
{
    outfile << min(image.left, label.left) << ","
            << min(image.top, label.top) << " " 
            << max(image.right, label.right) << ","
            << max(image.bottom, label.bottom) << "\t" << text << endl;
}

void 
Marker::setTransparency(unsigned char *pixel)
{
    transparency = true;
    memcpy(transparent, pixel, 3);
}

bool
Marker::isTransparent(const int pixel_x, const int pixel_y)
{
    if (!transparency || image_data == NULL) return(false);
    
    unsigned char *pixel = (image_data 
                            + 3 * (pixel_y * image_width + pixel_x));

    for (int i = 0; i < 3; i++)
        if (pixel[i] != transparent[i]) return(false);

    return(true);
}
