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

#ifndef DISPLAYX11_H
#define DISPLAYX11_H

#include <X11/Intrinsic.h>

#include "DisplayX11Base.h"

class DisplayX11 : public DisplayX11Base
{
 public:
    DisplayX11();

    void initialize();
    int getFontAscent();
    int getFontDescent();
    void getRGB(string color, unsigned char *RGB);
    int getTextHeight();
    int getTextHeight(string text);
    int getTextWidth(string text);
    void setFont(string fontname);
    void setFontSize(const int size);

 private:
// For label and markers
    enum {FontStruct, FontSet} fontmode;
    XFontSet fontset;
    XFontStruct *font;

    Pixel fg_pixel, bg_pixel;

    void doAnnotation(ProjectionBase *image);
    void drawImage(Marker &m);
    void drawCircle(const int x, const int y, const int r);
    void drawOutlinedString(const int x, const int y, const string text);
    void setColor(string color);
    Pixel ulongToPixel(const unsigned long value);
};

#endif
