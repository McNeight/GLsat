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

#ifndef DISPLAYX11BASE_H
#define DISPLAYX11BASE_H

#include <X11/Intrinsic.h>

#include "DisplayBase.h"

class DisplayX11Base : public DisplayBase
{
 public:
    DisplayX11Base();

    const char *identifyMe();
    void render(ProjectionBase *image);

 protected:
    Display *display;
    Colormap colormap;
    GC gc;
    Window window;
    Visual *visual;
    int depth;

    Pixmap pixmap;

    void computeShift(unsigned long mask, unsigned char &left_shift, 
                      unsigned char &right_shift);
    Pixmap createPixmap(const unsigned char *rgb_data, const int width,
                        const int height);
    void decomposePixmap(const Pixmap p, unsigned char *rgb_data);

    virtual void doAnnotation(ProjectionBase *image);
};

#endif
