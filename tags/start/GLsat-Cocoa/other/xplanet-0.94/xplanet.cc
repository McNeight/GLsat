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

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
using namespace std;

#include "auxfiles.h"
#include "checkfuncs.h"
#include "Options.h"
#include "Planet.h"

#include "util.h"

#include "libannotate/libannotate.h"
#include "libdisplay/libdisplay.h"
#include "libimage/libimage.h"
#include "libprojection/libprojection.h"

extern void moonpos(double &moonlon, double &moonlat, double &moondist,
                    const double T);
extern void doAnimate(int argc, char **argv, 
                      const ProjectionRectangular *image_rect);
extern int readSsecImage(string filename, int &width, int &height, 
                         unsigned char *&rgb_data);
unsigned char *elevation = NULL;

extern void addSpecularMap(const char *mapname, int width, int height, unsigned char *day);

int 
main(int argc, char **argv)
{ 
    // options passed to glut in doAnimate() call
    int glutargc = 1;
    char **glutargv = new char* [3];
    glutargv[0] = strdup(argv[0]);

    opts.processArgs(argc, argv, glutargc, glutargv);
    opts.setBody();

    double T = julianCentury((time_t &) opts.time.tv_sec);
    double eps = calcObliquity(T);

    sunpos(T);
    double sunlon = getSunLon();
    double sunlat = getSunLat();
    double sundist = getSunDist();

    double gst = TWO_PI * gmst(T, (time_t) opts.time.tv_sec) / 24;

    // coordinates of north pole, defaults to body's north pole, but
    // can be ecliptic or galactic
    double nlat = M_PI_2;
    double nlon = 0;

    if (opts.body == EARTH)
    {
        double alpha, delta;    // Equatorial coordinates of the sun
        eclipticToEquatorial(sunlon, sunlat, eps, alpha, delta);

        //  set sunlon and sunlat to subsolar longitude/latitude
        sunlon = alpha - gst;
        sunlat = delta;

        double moonlon, moonlat, moondist;
        moonpos(moonlon, moonlat, moondist, T);
        eclipticToEquatorial(moonlon, moonlat, eps, alpha, delta);
        setMoonLon(alpha - gst);
        setMoonLat(delta);

        switch (opts.north) {
        case ORBIT:
            nlon = drem(3*M_PI_2 - gst, TWO_PI);
            nlat = M_PI_2 - eps;
            break;
        case GALACTIC:
            equatorialToEcliptic(GN_LON, GN_LAT, eps, nlon, nlat);
            nlon = drem(nlon - gst, TWO_PI);
            break;
        case BODY:
        default:
            break;
        }

        if (!opts.satellitefile.empty()) createSatelliteList();
    }
    else 
    {
        Planet selected(T, opts.body);
        if (opts.body == MOON)
        {
            selected.helio.lon = sunlon + M_PI;
            selected.helio.lat = sunlat;
            selected.helio.dist = sundist;
        }
        else
            selected.calcHeliocentric();

        coordinates subsolar = selected.calcCoordinates(selected.helio, eps);

        switch (opts.north) {
        case ORBIT:
            selected.calcEcliptic(eps, nlat, nlon);
            break;
        case GALACTIC:
            selected.calcGalactic(nlat, nlon);
            break;
        case BODY:
        default:
            break;
        }

        if (opts.position == EARTHSIDE)
        {
            if (opts.body == MOON)
                moonpos(selected.geo.lon, selected.geo.lat,
                        selected.geo.dist, T);
            else
                selected.calcGeocentric(sunlon, sundist);

            coordinates subearth = selected.calcCoordinates(selected.geo, 
                                                            eps);

            // correction for light_time
            opts.setObsLon(subearth.lon + subearth.alpha); 
            opts.setObsLat(subearth.lat);

            subsolar.lon += subearth.alpha;  // correction for light_time
//          opts.rotate += subearth.dist; // position angle of N pole 
        }

        sunlon = subsolar.lon;
        sunlat = subsolar.lat;
    }

    setSunLat(sunlat);
    setSunLon(sunlon);

    // Set the observer position
    opts.setPosition();

    if (opts.north != BODY)
    {
        double trueCourse, dist;
        calcGreatArc(opts.getObsLon(), opts.getObsLat(), nlon, nlat, 
                     trueCourse, dist);
        opts.rotate -= trueCourse;
    }

    DisplayBase *thisDisplay = getDisplay(opts.truetype);

    // set up color and fonts for annotation, if needed
    thisDisplay->initialize();

    int window_width = thisDisplay->getWidth();
    int window_height = thisDisplay->getHeight();
    int window_area = window_width * window_height;

    int image_width, image_height;
    int i;

    if (opts.projection == RECTANGULAR)
    {
        image_width = window_width;
        image_height = window_height;
    } 
    else // set dimensions to image file's dimensions
    {
        image_width = -1;
        image_height = -1;
    }

#ifdef HAVE_ANIMATION_LIB
    if (opts.display == ANIMATE)
    {
#ifdef __sparc__
        image_width = 512;
        image_height = 512;
#else
        image_width = 1024;
        image_height = 1024;
#endif
    }
#endif

    unsigned char *day = NULL;
    unsigned char *night = NULL;

    if (opts.day.empty())
    {
        opts.day = body_string[opts.body];
        opts.day += defaultmapext;
    }

    if (!readImageFile(opts.day, image_width, image_height, day))
    {
        cerr << "Can't read day image file " << opts.day << ".\n\n";
        cerr << "Map files may be obtained from the sources listed at\n";
        cerr << "http://xplanet.sourceforge.net.\n\n";
        cerr << "By default, xplanet will look for maps in the "
             << "following order:\n";
        
        if (opts.mapdir.size() > 0)
        {
            list<string>::iterator p = opts.mapdir.begin();
            while (p != opts.mapdir.end())
            {
                cerr << *p << endl;
                p++;
            }
            
        }

        cerr << "the current directory\nthe images subdirectory\n" 
             << prefix << separator << "images\n";
        
        if (opts.mapdir.size() == 0)
            cerr << "\nUse the -mapdir option to specify an alternate "
                 << "directory.\n";
        
        exit(EXIT_FAILURE);
    }

    int image_area = image_width * image_height;

    int have_image = 0;
    if (!opts.night.empty()) 
    {
        have_image = readImageFile(opts.night, image_width,
                                   image_height, night);
        if (!have_image)
            cerr << "Can't read night image file " << opts.night << "\n";
    }

    if (!have_image)
    {
        /*
          Read in the default night image, if the planet is earth.  We
          don't want to do this if:

          1) shade is 1 or cloud_shade is set, as the user probably
             wants to generate an image with clouds overlaid.

          2) mapbounds is set, as there is no default night image in
             this case.

          3) The user specified an image with --night_image and it
             wasn't found.  In that case, just use a dimmed day map.
        */

        if (opts.body == EARTH 
            && opts.shade < 1 && !opts.cloud_shade
            && !opts.mapbounds
            && opts.night.empty())
        { 
            string defaultnight = "night";
            defaultnight += defaultmapext;
            have_image = readImageFile(defaultnight, image_width,
                                       image_height, night); 
        }


        // Didn't load a separate night image, so use a shaded day map.
        if (!have_image)
        {
            // I use malloc here because that's what read_image_file uses
            night = (unsigned char *) malloc(3 * image_area);
            if (night == NULL)
            {
                cerr << "Can't allocate memory for night image\n";
                exit(EXIT_FAILURE);
            }

            memcpy(night, day, 3 * image_area); 
            if (!opts.cloud_shade)
            {
                for (i = 0; i < 3 * image_area; i++) 
                    night[i] = (unsigned char) (night[i] * opts.shade);
            }
        }
    }
  
    if (!opts.specular_file.empty() && opts.projection == ORTHOGRAPHIC)
        addSpecularMap(opts.specular_file.c_str(), image_width, image_height, day);

    have_image = 0;
    if (!opts.cloud.empty())
    {
        unsigned char *cloud = NULL;
        if (opts.ssec)
            have_image = readSsecImage(opts.cloud, image_width,
                                       image_height, cloud);
        else
            have_image = readImageFile(opts.cloud, image_width,
                                       image_height, cloud);

        if (have_image)
        {
            // overlay the cloud image onto the day & night maps
            double dayshade = 1;
            if (opts.cloud_shade) dayshade = opts.shade;

            if (opts.cloud_gamma != 1)
            {
                unsigned char *map = new unsigned char[256];
                if (opts.cloud_gamma > 0)
                {
                    for (i = 0; i < 256; i++)
                    {
                        map[i] = ((unsigned char) 
                                  (pow(((double) i) / 255, 
                                       1.0/opts.cloud_gamma) * 255));
                    }
                }
                else
                {
                    memset(map, 0, 256);
                }

                for (i = 0; i < 3 * image_area; i++)
                    cloud[i] = map[cloud[i]];

                delete [] map;
            }         
          
            for (i = 0; i < image_area; i++) 
            {
                int ipos = 3 * i;
                if ((int) cloud[ipos] >= opts.cloud_threshold)
                {
                    for (int j = ipos; j < ipos + 3; j++)
                    {
                        double opacity = ((double) cloud[j]) / 255;
                        float color = (opacity * dayshade * cloud[j]
                                       + (1-opacity) * day[j]);
                        day[j] = (unsigned char) color;
                        color = (opacity * opts.shade * cloud[j] 
                                 + (1-opacity) * night[j]);
                        night[j] = (unsigned char) color;
                    }
                }
            }
            deleteImage(cloud);
        }
        else
            cerr << "Can't read cloud image file " << opts.cloud << "\n";
    }

    if (!opts.dem.empty())
    {
        unsigned char *dem = NULL;

        if (readImageFile(opts.dem, image_width, image_height, dem))
        {
            // assume digital elevation map is made of 1 byte values
            elevation = new unsigned char[image_area];
          
            for (i = 0; i < image_height * image_width; i++)
                elevation[i] = dem[3*i];
            deleteImage(dem);
        }
        else
            cerr << "Can't read DEM image file " << opts.dem << "\n";
    }

    /*
      This object will hold the day/night image used to create the other 
      projections.  
    */
    ProjectionRectangular *image_rect;
    image_rect = new ProjectionRectangular(image_width, image_height);
    image_rect->createProjection(sunlon, sunlat, day, night);

    deleteImage(day);
    deleteImage(night);

#ifdef HAVE_ANIMATION_LIB
    if (opts.display == ANIMATE) 
    {
        if (opts.grid1) image_rect->drawGrid();
        if (!opts.greatarcfile.empty()) drawGreatArcs(image_rect, NULL);

        doAnimate(glutargc, glutargv, image_rect);
    }
#endif

    ProjectionBase *image = NULL;

    if (opts.projection == RECTANGULAR) 
    {
        image = image_rect;
    }
    else
    {
        image = getProjection(opts.projection, window_width, window_height);
        image->createProjection(image_rect);
        delete image_rect;
    }

    // Center rectangular projection at a longitude other than 0
    if (opts.projection == RECTANGULAR && opts.getObsLon() != 0 
        && !opts.mapbounds)
    {
        unsigned char *temp = new unsigned char[3 * window_area];
        memcpy(temp, image->rgb_data, 3 * window_area);

        int istart = (int) (window_width * (1 - opts.getObsLon() / (2*M_PI)));
        if (istart >= window_width) istart -= window_width;

        for (i = 0; i < window_height; i++)
        {
            memcpy(image->rgb_data + 3 * i * window_width, 
                   temp + 3 * ((i+1) * window_width - istart), 
                   3*istart);
            memcpy(image->rgb_data + 3 * (istart + i * window_width), 
                   temp + 3 * (i * window_width), 3*(window_width - istart));
        }
        delete [] temp;
    }
    
    // set PNG alpha (opacity) channel
    set_alpha(image->png_alpha);

    // set JPEG quality
    if (opts.display == OUTPUT) set_quality(opts.quality);

    thisDisplay->render(image);

    if (!opts.satellitefile.empty()) 
        cleanupSatelliteFiles(thisDisplay->getTempdir());

    delete thisDisplay;
    delete image;

    exit(EXIT_SUCCESS);
}
