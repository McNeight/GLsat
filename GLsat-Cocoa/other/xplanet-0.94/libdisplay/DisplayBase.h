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

#ifndef DISPLAYBASE_H
#define DISPLAYBASE_H

#include <map>
#include <string>
using namespace std;

#include "checkfuncs.h"

#ifdef HAVE_LIBFREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

class Marker;
class ProjectionBase;

class DisplayBase
{
 public:
    static int getType();

    virtual ~DisplayBase();

    int getHeight();
    int getWidth();

// routines which must be defined in derived classes
    virtual const char *identifyMe() = 0;
    virtual void render(ProjectionBase *image) = 0;

// routines to be optionally overridden by derived classes
    virtual void initialize();
    virtual void drawMarker(Marker &m);
    virtual int getFontAscent();
    virtual int getFontDescent();
    virtual void getRGB(string color, unsigned char *RGB);
    virtual string getTempdir();
    virtual int getTextHeight(const string text);
    virtual int getTextWidth(const string text);
    virtual void setFont(string fontname);
    virtual void setFontSize(const int size);

 protected:
    int window_width, window_height;
    int window_area;

    unsigned char *png_alpha;
    unsigned char *rgb_data;
    unsigned char bg[3]; // background color for annotation
    unsigned char fg[3]; // foreground color for annotation

    void createLabel();
    void doAnnotation(ProjectionBase *image);
    void loadRGB();
    void setAlpha(unsigned char *c);
    void setImage(unsigned char *c);

// routines to be optionally overridden by derived classes
    virtual int fontExists(string &filename);
    virtual void drawCircle(const int x, const int y, const int r);
    virtual void drawImage(Marker &m);
    virtual void drawLabel(const string body_label, const string date_label,
                           const string illum_label, const string obs_label,
                           const string rotate_label, const string sun_label);
    virtual void drawOutlinedString(const int x, const int y, 
                                    const string text);
    virtual void printError(const string error);
    virtual void setColor(string color);
    virtual void setForeground(unsigned char *color);
    virtual void setPixel(const int x, const int y);
    virtual void setPixel(const int x, const int y, double opacity);
    virtual int getTextHeight();

 private:
#ifdef HAVE_LIBFREETYPE
    FT_Library library;
    FT_Face face; 
    int fontsize;
#endif

    unsigned char current_color[3];
    map<string, unsigned int> RGBColors;

    void drawText(const int x, const int y, const string text);
};

#endif
