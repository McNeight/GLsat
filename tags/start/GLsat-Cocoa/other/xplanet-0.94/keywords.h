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

#ifndef KEYWORDS_H
#define KEYWORDS_H

enum body_name
{
    MERCURY, VENUS, EARTH, MOON, MARS, 
    JUPITER, IO, EUROPA, GANYMEDE, CALLISTO,
    SATURN, TITAN, URANUS, NEPTUNE, PLUTO,
    RANDOMBODY
};

enum keyWords
{
    UNKNOWN = '?',            // for getopt
    ABOVE, ALIGN, ALTCIRC, ANCIENT, ANIMATE, AQUA, ARC, AUTO, AZIMUTHAL, 
    BACKGROUND, BELOW, BLEND, BODY, 
    CENTER, CLOUD_IMAGE, CLOUD_GAMMA, CLOUD_SHADE, CLOUD_SSEC, 
    CLOUD_THRESHOLD, COLOR, 
    DATE, DAYSIDE, DEBUG, DELIMITER, DEMFILE, DEMSCALE, 
    EARTHSIDE, ENDOFLINE, EVENING, EXISTINGWINDOW, 
    FONT, FONTDIR, FONTSIZE, FULLSCREEN, FUZZ, 
    GALACTIC, GEOMETRY, GMTLABEL, GREATARCFILE,
    GRID, GRID1, GRID2, GROUND,
    HELP, HEMISPHERE, 
    IMAGE, 
    LABEL, LABELFMT, LABELNAME, LABELPOS, LABELROT, LATITUDE, 
    LATLON, LEFT, LOCALTIME, LONGITUDE, 
    MAGICK, MAPBOUNDS, MAPDIR, MARKERBOUNDS, MARKERFILE, MARKERS, MERCATOR, 
    MOLLWEIDE, MOONSIDE, MORNING, MSWIN,
    NAME, NIGHT_IMAGE, NIGHTSIDE, NORTH, NOTRANSPARENCY,
    OBSERVER, ORBIT, ORTHOGRAPHIC, OUTPUT, 
    PETERS, POSITION, PRINT_COORDS, PROJECTION, 
    QUALITY,
    RADIUS, RANDOM, RANGE, RECTANGULAR, RIGHT, ROOT, ROTATE, 
    SATELLITE, SATFILE, SATTRACKID, SHADE, SPACING, SPECULAR_FILE, 
    STARFREQ, SUNREL, SWAP, SYMBOLSIZE,
    TERMINATOR, TIMEZONE, TRAIL, TRANSPARENT, TRANSPNG, TRUETYPE, 
    VERSION, VROOT,
    WINDOW,
    X11
};

#endif
