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

#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

#include "DisplayOutput.h"
#include "Options.h"

#include "libimage/libimage.h"
#include "libprojection/libprojection.h"

DisplayOutput::DisplayOutput()
{
    window_width = opts.window_width;
    window_height = opts.window_height;
  
    // (x,y) of the globe's center
    if (!opts.center)
    {
        opts.centx = window_width / 2;
        opts.centy = window_height / 2;
    }
  
    window_area = window_width * window_height;
}

const char *
DisplayOutput::identifyMe()
{
    return("OUTPUT");
}

void 
DisplayOutput::render(ProjectionBase *image)
{
    doAnnotation(image);

    // Swap red & blue planes if desired.  This is useful on
    // big-endian machines.
    if (opts.swap)
    {
        unsigned char tmp;
      
        for (int i = 0; i < window_area; i++) 
        {
            tmp         = image->rgb_data[3*i];
            image->rgb_data[3*i]   = image->rgb_data[3*i+2]; 
            image->rgb_data[3*i+2] = tmp;
        }
    }
  
    if (!write_image(opts.output.c_str(), window_width, window_height,
                     image->rgb_data))
    {
        cerr << "Can't create image file " << opts.output << "\n";
        exit(EXIT_FAILURE);
    }

    return;
}
