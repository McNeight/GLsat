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

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

#include "DisplayMSWin.h"
#include "Options.h"

#include "libannotate/libannotate.h"
#include "libimage/libimage.h"
#include "libprojection/libprojection.h"

#include <windows.h>

DisplayMSWin::DisplayMSWin()
{
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    if (opts.fullscreen) 
    {
        opts.window_width = screen_width;
        opts.window_height = screen_height;
    }

    switch (opts.display)
    {
    case WINDOW:
        cerr << "-window option not supported for " << identifyMe() 
             << " mode.\n";
        opts.display = ROOT;
        // fall through
    case ROOT:
        window_width = screen_width;
        window_height = screen_height;
        break;
    case OUTPUT:
        window_width = opts.window_width;
        window_height = opts.window_height;
        break;
    }
  
    // (x,y) of the globe's center
    if (!opts.center)
    {
        opts.centx = window_width / 2;
        opts.centy = window_height / 2;
    }
  
    window_area = window_width * window_height;
}

const char *
DisplayMSWin::identifyMe()
{
    return("MSWin");
}

void 
DisplayMSWin::render(ProjectionBase *image)
{
    doAnnotation(image);

    switch (opts.display)
    {
    case OUTPUT:
    {
        if (!write_image(opts.output.c_str(), window_width, window_height, 
                         image->rgb_data))
        {
            cerr << "Can't create image file " << opts.output << "\n";
            exit(EXIT_FAILURE);
        }
    }
    break;
    case ROOT:
    {
        string tmpdir = getTempdir();
        tmpdir += "\\XPlanet.bmp";
        
        if (!write_image(tmpdir.c_str(), window_width, window_height, 
                         image->rgb_data))
        {
            cerr << "Can't create bitmap file.\n";
            exit(EXIT_FAILURE);
        }
        
        // Tell Windows to update the desktop wallpaper
        SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, 
                             (char *) tmpdir.c_str(), 
                             SPIF_UPDATEINIFILE);
    }
    break;
    }

    return;
}

string
DisplayMSWin::getTempdir()
{
    char tmpdir[MAX_PATH];
    GetWindowsDirectory(tmpdir, MAX_PATH);
    return(string(tmpdir));
}
