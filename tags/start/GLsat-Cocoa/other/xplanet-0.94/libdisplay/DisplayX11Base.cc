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

#include <climits>
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;

#include <X11/keysym.h>
#include <X11/Xatom.h>

#include "auxfiles.h"
#include "DisplayX11Base.h"
#include "Marker.h"
#include "Options.h"
#include "util.h"

#include "libannotate/libannotate.h"
#include "libimage/libimage.h"
#include "libprojection/libprojection.h"

#include "vroot.h"

DisplayX11Base::DisplayX11Base()
{
    display = XOpenDisplay(NULL);
    if (display == NULL)
    { 
        cerr << "Can't open X display\n";
        exit(EXIT_FAILURE);
    }

    int screen_num    = DefaultScreen(display);
    int screen_width  = DisplayWidth(display, screen_num);
    int screen_height = DisplayHeight(display, screen_num);

    Window root;

    if (opts.vroot)
    {
        root = VirtualRootWindowOfScreen(ScreenOfDisplay(display,
                                                         screen_num));
    }
    else
    {
        root = ScreenOfDisplay(display, screen_num)->root;
    }

    colormap = DefaultColormap(display, screen_num);
    depth    = DefaultDepth(display, screen_num);
    gc       = XCreateGC(display, root, 0, NULL);
    visual   = DefaultVisual(display, screen_num);

    if (opts.fullscreen) 
    {
        opts.window_width = screen_width;
        opts.window_height = screen_height;
        if (opts.display == ROOT) opts.display = WINDOW;
    }

    switch (opts.display)
    {
    case ROOT:
        window_width = screen_width;
        window_height = screen_height;
        window = root;
        break;
    case WINDOW:
        window_width = opts.window_width;
        window_height = opts.window_height;
        if (window_width > screen_width) 
        {
            window_width = screen_width;
            cerr << "Width set to " << window_width << endl;
        }
        if (window_height > screen_height) 
        {
            window_height = screen_height;
            cerr << "Height set to " << window_height << endl;
        }

        // Check if we've already specified a window to use. 
        // If not, create one.
        if (opts.XID)
        {
            window = opts.XID;
        }
        else
        {
            if (opts.window_mask & XNegative) 
                opts.window_x += (screen_width - window_width);
            if (opts.window_mask & YNegative) 
                opts.window_y += (screen_height - window_height);

            window = XCreateSimpleWindow(display, root, opts.window_x, 
                                         opts.window_y, window_width,
                                         window_height, 4, 
                                         WhitePixel(display, screen_num),
                                         BlackPixel(display, screen_num));

            if ((opts.window_mask & XValue) 
                || (opts.window_mask & YValue)
                || (opts.window_mask & XNegative) 
                || (opts.window_mask & YNegative))
            {
                XSizeHints *hints = XAllocSizeHints();
                hints->flags = USPosition;
                XSetWMNormalHints(display, window, hints);
            }

            XTextProperty windowName;
            if (opts.title.empty()) opts.title = versionstring;
            char *title = (char *) opts.title.c_str();
            XStringListToTextProperty(&title, 1, &windowName);
            XSetWMName(display, window, &windowName);
        }      
        break;
    case OUTPUT:
        window_width = opts.window_width;
        window_height = opts.window_height;
        window = root;
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
DisplayX11Base::identifyMe()
{
    return("X11");
}

void
DisplayX11Base::doAnnotation(ProjectionBase *image)
{
    if (opts.grid1) image->drawGrid();

    // read the list of satellites to plot and write the corresponding
    // marker/arc files
    if (!opts.satellitefile.empty()) createSatelliteFiles(getTempdir());

    if (!opts.greatarcfile.empty()) drawGreatArcs(image, this);
 
    setAlpha(image->png_alpha);
    setImage(image->rgb_data);
    
    if (!opts.markerfile.empty()) drawMarkers(image, this);

    if (opts.body == SATURN) image->drawRings();

    if (opts.label) createLabel();

    pixmap = createPixmap(image->rgb_data, window_width, window_height);
}

void 
DisplayX11Base::render(ProjectionBase *image)
{
    doAnnotation(image);

    switch (opts.display)
    {
    case OUTPUT:
    {
        unsigned char *temp = new unsigned char[3 * window_area];
        decomposePixmap(pixmap, temp);
            
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
                         temp))
        {
            cerr << "Can't create image file " << opts.output << "\n";
            exit (EXIT_FAILURE);
        }
        
        delete [] temp;
        return;
    }
    case WINDOW:
    {
        int icon_width = 64;
        int icon_height = 64;
        
        unsigned char *temp = new unsigned char[3 * window_area];
        decomposePixmap(pixmap, temp);

        resize_image(&temp, window_width, window_height, icon_width, 
                     icon_height, 0);

        Pixmap icon_pixmap = createPixmap(temp, icon_width, icon_height);
        
        delete [] temp;
        
        XWMHints *wm_hints = XAllocWMHints();
        wm_hints->flags = IconPixmapHint;
        wm_hints->icon_pixmap = icon_pixmap;
        XSetWMHints(display, window, wm_hints);
        XMapWindow(display, window);
    }
    break;
    case ROOT:
        if (opts.trans)
        {
            // Set the background pixmap for Eterms and aterms.  Most
            // of this is taken from the Esetroot source code.
              
            Atom prop_root = XInternAtom(display, "_XROOTPMAP_ID", True);
            Atom prop_esetroot = XInternAtom(display, "ESETROOT_PMAP_ID", 
                                             True);
              
            if (prop_root != None && prop_esetroot != None)
            {
                Atom type;
                int format;
                unsigned long length, after;
                unsigned char *data_root;
                unsigned char *data_esetroot;
                  
                XGetWindowProperty(display, window, prop_root, 0L, 1L, 
                                   False, AnyPropertyType, &type, 
                                   &format, &length, &after, &data_root);
                if (type == XA_PIXMAP) 
                {
                    XGetWindowProperty(display, window, prop_esetroot, 
                                       0L, 1L, False, AnyPropertyType, 
                                       &type, &format, &length, 
                                       &after, &data_esetroot);
                    if (data_root && data_esetroot && type == XA_PIXMAP 
                        && (*((Pixmap *) data_root) 
                            == *((Pixmap *) data_esetroot)))
                        XKillClient(display, *((Pixmap *) data_root));
                }
            }
              
            prop_root = XInternAtom(display, "_XROOTPMAP_ID", False);
            prop_esetroot = XInternAtom(display, "ESETROOT_PMAP_ID", False);
              
            XChangeProperty(display, window, prop_root, XA_PIXMAP, 32, 
                            PropModeReplace, (unsigned char *) &pixmap, 1);
            XChangeProperty(display, window, prop_esetroot, XA_PIXMAP, 32, 
                            PropModeReplace, (unsigned char *) &pixmap, 1);
            XSetCloseDownMode(display, RetainPermanent);
        }
        break;
    default:
        break;
    }
      
    XSetWindowBackgroundPixmap(display, window, pixmap);
    XClearWindow(display, window);
    XFlush(display);

    if (opts.display == WINDOW && !opts.XID)
    {
        Atom wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", 
                                          False);
        XSetWMProtocols(display, window, &wmDeleteWindow, 1);

        XSelectInput(display, window, KeyPressMask);
          
        // now listen for events
        XEvent event;
        while(1)
        {
            XNextEvent(display, &event);
            switch(event.type)
            {
            case ClientMessage:
                if ((unsigned int) event.xclient.data.l[0] == wmDeleteWindow) 
                {
                    XCloseDisplay(display);
                    return;
                }
                break;
            case KeyPress:
                KeySym keysym;
                char keybuf;
                XLookupString (&(event.xkey), &keybuf, 1, &keysym, NULL);
                if (keybuf == 'q' || keybuf == 'Q') 
                {
                    XCloseDisplay(display);
                    return;
                }
                break;
            default:
                break;
            }
        }
    }
    XCloseDisplay(display);
} 

void
DisplayX11Base::computeShift(unsigned long mask, 
                             unsigned char &left_shift, 
                             unsigned char &right_shift)
{
    left_shift = 0;
    right_shift = 8;
    if (mask != 0)
    {
        while ((mask & 0x01) == 0)
        {
            left_shift++;
            mask >>= 1;
        }
        while ((mask & 0x01) == 1)
        {
            right_shift--;
            mask >>= 1;
        }
    }
}

Pixmap
DisplayX11Base::createPixmap(const unsigned char *rgb, 
                             const int width, const int height)
{
    int i, j;   // loop variables 

    Pixmap tmp = XCreatePixmap(display, window, width, height, depth);

    char *pixmap_data = NULL;
    switch (depth)
    {
    case 32:
    case 24:
        pixmap_data = new char[4 * width * height];
        break;
    case 16:
    case 15:
        pixmap_data = new char[2 * width * height];
        break;
    case 8:
        pixmap_data = new char[width * height];
        break;
    default:
        break;
    }

    XImage *ximage = XCreateImage(display, visual, depth, ZPixmap, 0,
                                  pixmap_data, width, height, 8, 0);

    XVisualInfo visual_info;
    int entries;
    XVisualInfo v_template;
    v_template.visualid = XVisualIDFromVisual(visual);
    visual_info = *XGetVisualInfo(display, VisualIDMask, &v_template,
                                  &entries);

    unsigned long ipos = 0;
    switch (visual_info.c_class)
    {
    case PseudoColor:  
    {
        XColor xc;
        xc.flags = DoRed | DoGreen | DoBlue;

        int num_colors = 256;
        XColor *colors = new XColor[num_colors];
        for (i = 0; i < num_colors; i++) colors[i].pixel = (unsigned long) i;
        XQueryColors(display, colormap, colors, num_colors);
        
        int *closest_color = new int[num_colors];

        for (i = 0; i < num_colors; i++)
        {
            xc.red = (i & 0xe0) << 8;           // highest 3 bits
            xc.green = (i & 0x1c) << 11;        // middle 3 bits
            xc.blue = (i & 0x03) << 14;         // lowest 2 bits

            // find the closest color in the colormap
            double distance, distance_squared, min_distance = 0;
            for (int ii = 0; ii < num_colors; ii++)
            {
                distance = colors[ii].red - xc.red;
                distance_squared = distance * distance;
                distance = colors[ii].green - xc.green;
                distance_squared += distance * distance;
                distance = colors[ii].blue - xc.blue;
                distance_squared += distance * distance;
                
                if ((ii == 0) || (distance_squared <= min_distance))
                {
                    min_distance = distance_squared;
                    closest_color[i] = ii;
                }
            }
        }

        for (j = 0; j < height; j++)
        {
            for (i = 0; i < width; i++)
            {
                xc.red = (unsigned short) (rgb[ipos++] & 0xe0);
                xc.green = (unsigned short) (rgb[ipos++] & 0xe0);
                xc.blue = (unsigned short) (rgb[ipos++] & 0xc0);

                xc.pixel = xc.red | (xc.green >> 3) | (xc.blue >> 6);
                XPutPixel(ximage, i, j, 
                          colors[closest_color[xc.pixel]].pixel);
            }
        }
        delete [] colors;
        delete [] closest_color;
    }
    break;
    case TrueColor:
    {
        unsigned char red_left_shift;
        unsigned char red_right_shift;
        unsigned char green_left_shift;
        unsigned char green_right_shift;
        unsigned char blue_left_shift;
        unsigned char blue_right_shift;

        computeShift(visual_info.red_mask, red_left_shift, 
                     red_right_shift);
        computeShift(visual_info.green_mask, green_left_shift, 
                     green_right_shift);
        computeShift(visual_info.blue_mask, blue_left_shift, 
                     blue_right_shift);

        unsigned long pixel;
        unsigned long red, green, blue;
        for (j = 0; j < height; j++)
        {
            for (i = 0; i < width; i++)
            {
                red = (unsigned long) 
                    rgb[ipos++] >> red_right_shift;
                green = (unsigned long) 
                    rgb[ipos++] >> green_right_shift;
                blue = (unsigned long) 
                    rgb[ipos++] >> blue_right_shift;
                
                pixel = (((red << red_left_shift) & visual_info.red_mask)
                         | ((green << green_left_shift) 
                            & visual_info.green_mask)
                         | ((blue << blue_left_shift) 
                            & visual_info.blue_mask));

                XPutPixel(ximage, i, j, pixel);
            }
        }
    }
    break;
    default:
        cerr << "create_pixmap: visual = " << visual_info.c_class << endl;
        cerr << "Visual should be either PseudoColor or TrueColor\n";
        return(tmp);
    }
          
    XPutImage(display, tmp, gc, ximage, 0, 0, 0, 0, width, height);

    delete [] pixmap_data;
    pixmap_data = NULL;

    return(tmp);
}

void
DisplayX11Base::decomposePixmap(const Pixmap p, unsigned char *rgb)
{
    int i, j;
    unsigned long ipos = 0;

    XVisualInfo visual_info;
    int entries;
    XVisualInfo v_template;
    v_template.visualid = XVisualIDFromVisual(visual);
    visual_info = *XGetVisualInfo(display, VisualIDMask, &v_template, 
                                  &entries);

    XImage *ximage = XGetImage(display, p, 0, 0, window_width, 
                               window_height, AllPlanes, ZPixmap);

    switch (visual_info.c_class)
    {
    case PseudoColor:
    {
        XColor *xc = new XColor[window_width];

        for (j = 0; j < window_height; j++)
        {
            for (i = 0; i < window_width; i++)
                xc[i].pixel = XGetPixel(ximage, i, j);

            XQueryColors(display, colormap, xc, window_width);

            for (i = 0; i < window_width; i++)
            {
                rgb[ipos++] = (unsigned char) (xc[i].red >> 8);
                rgb[ipos++] = (unsigned char) (xc[i].green >> 8);
                rgb[ipos++] = (unsigned char) (xc[i].blue >> 8);
            }
        }
        delete [] xc;
    }
    break;
    case TrueColor:
    {
        unsigned char red_left_shift;
        unsigned char red_right_shift;
        unsigned char green_left_shift;
        unsigned char green_right_shift;
        unsigned char blue_left_shift;
        unsigned char blue_right_shift;
        
        computeShift(visual_info.red_mask, red_left_shift, 
                     red_right_shift);
        computeShift(visual_info.green_mask, green_left_shift, 
                     green_right_shift);
        computeShift(visual_info.blue_mask, blue_left_shift, 
                     blue_right_shift);
        
        unsigned long pixel;
        for (j = 0; j < window_height; j++)
            for (i = 0; i < window_width; i++)
            {
                pixel = XGetPixel(ximage, i, j);
                rgb[ipos++] = ((unsigned char) 
                               (((pixel & visual_info.red_mask) 
                                 >> red_left_shift)
                                << red_right_shift));
                rgb[ipos++] = ((unsigned char) 
                               (((pixel & visual_info.green_mask) 
                                 >> green_left_shift)
                                << green_right_shift));
                rgb[ipos++] = ((unsigned char) 
                               (((pixel & visual_info.blue_mask) 
                                 >> blue_left_shift)
                                << blue_right_shift));
            }
    }
    break;
    default:
        cerr << "decomposePixmap: visual = " << visual_info.c_class << endl;
        cerr << "Visual should be either PseudoColor or TrueColor\n";
        return;
    }
}
