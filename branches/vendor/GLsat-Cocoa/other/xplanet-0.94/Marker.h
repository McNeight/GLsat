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

#ifndef MARKER_H
#define MARKER_H

#include <fstream>
#include <string>
using namespace std;

class Marker
{
 public:
    int x, y;
    int image_width, image_height;
    unsigned char *image_data;

    int text_x, text_y;
    int text_width, text_height;
    string text;

    bool fixed_align;
    int align;

    string color;
    string font;
    int fontsize;

    int ID;
    
    Marker(const int X, const int Y,
           const string Name, const int Align);

    ~Marker();

    bool operator < (const Marker &m);
    bool operator > (const Marker &m);
    bool operator == (const Marker &m);
    bool operator != (const Marker &m);

    bool isTransparent(const int pixel_x, const int pixel_y);
    int overlap(const Marker &m);

    void printCorners(ofstream &outfile);
    void setImage(const int width, const int height);

    void setImage(string filename);

    void setCorners(const int x1, const int y1, 
                    const int x2, const int y2);
    void setTransparency(unsigned char *pixel);

 private:
    bool transparency;
    unsigned char transparent[3];

    struct BoundingBox
    {
        int top, bottom, left, right; 
    };
        
    BoundingBox image, label;

    int overlap(const BoundingBox &b);

};

#endif
