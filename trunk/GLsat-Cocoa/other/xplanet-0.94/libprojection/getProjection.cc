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

#include <cctype>
#include <iostream>
#include <cstring>
using namespace std;

#include "keywords.h"

#include "ProjectionBase.h"
#include "ProjectionAncient.h"
#include "ProjectionAzimuthal.h"
#include "ProjectionHemisphere.h"
#include "ProjectionMollweide.h"
#include "ProjectionMercator.h"
#include "ProjectionOrthographic.h"
#include "ProjectionPeters.h"
#include "ProjectionRectangular.h"

int 
getProjectionType(char *proj_string)
{
    int projection;

    char *lowercase = proj_string;
    char *ptr = proj_string;
    while (*ptr != '\0') *ptr++ = tolower(*proj_string++);
    if (strncmp(lowercase, "ancient", 2) == 0)
        projection = ANCIENT;
    else if (strncmp(lowercase, "azimuthal", 2) == 0)
        projection = AZIMUTHAL;
    else if (strncmp(lowercase, "hemisphere", 1) == 0)
        projection = HEMISPHERE;
    else if (strncmp(lowercase, "mercator", 2) == 0)
        projection = MERCATOR;
    else if (strncmp(lowercase, "mollweide", 2) == 0)
        projection = MOLLWEIDE;
    else if (strncmp(lowercase, "orthographic", 1) == 0)
        projection = ORTHOGRAPHIC;
    else if (strncmp(lowercase, "peters", 1) == 0)
        projection = PETERS;
    else if (strncmp(lowercase, "rectangular", 1) == 0)
        projection = RECTANGULAR;
    else 
    {
        cerr << "Unknown projection, using rectangular\n";
        projection = RECTANGULAR;
    }
    return(projection);
}

ProjectionBase *
getProjection(const int projection, const int width, const int height)
{
    ProjectionBase *thisProjection = NULL;
    switch (projection)
    {
    case ANCIENT:
        thisProjection = new ProjectionAncient(width, height);
        break;
    case AZIMUTHAL:
        thisProjection = new ProjectionAzimuthal(width, height);
        break;
    case HEMISPHERE:
        thisProjection = new ProjectionHemisphere(width, height);
        break;
    case MERCATOR:
        thisProjection = new ProjectionMercator(width, height);
        break;
    case MOLLWEIDE:
        thisProjection = new ProjectionMollweide(width, height);
        break;
    case ORTHOGRAPHIC:
        thisProjection = new ProjectionOrthographic(width, height);
        break;
    case PETERS:
        thisProjection = new ProjectionPeters(width, height);
        break;
    default:
        break;
    }
    return(thisProjection);
}
