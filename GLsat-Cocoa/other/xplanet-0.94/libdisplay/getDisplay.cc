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

#include "checkfuncs.h"
#include "keywords.h"

#include "DisplayBase.h"
#include "DisplayOutput.h"

#ifdef HAVE_LIBX11
#include "DisplayX11Base.h"
#include "DisplayX11.h"
#endif

#ifdef HAVE_AQUA
#include "DisplayMacAqua.h"
#endif

#ifdef HAVE_CYGWIN
#include "DisplayMSWin.h"
#endif

DisplayBase *getDisplay(bool truetype)
{
    DisplayBase *thisDisplay = NULL;

    switch (DisplayBase::getType())
    {
#ifdef HAVE_AQUA        
    case AQUA:
        thisDisplay = new DisplayMacAqua();     
        break;
#endif
#ifdef HAVE_CYGWIN
    case MSWIN:
        thisDisplay = new DisplayMSWin();
        break;
#endif
    case OUTPUT:
        thisDisplay = new DisplayOutput();
        break;
#ifdef HAVE_LIBX11
    case X11:
        if (truetype)
            thisDisplay = new DisplayX11Base();
        else
            thisDisplay = new DisplayX11();
        break;
#endif
    default:
    case UNKNOWN:
        std::cerr << "Unknown display type.\n";
        exit(EXIT_FAILURE);
        break;
    }
    return(thisDisplay);
}
