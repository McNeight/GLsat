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

#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;

#include "keywords.h"

bool
isDelimiter(char c)
{
    return(c == ' ' || c == '\t' || c == ',' || c == '/');
}

bool
isEndOfLine(char c)
{
    // 13 is DOS end-of-line, 28 is the file separator
    return(c == '#' || c == '\0' || c == 13 || c == 28); 
}

static void
skipPastToken(int &i, const char *line)
{
    while (!isDelimiter(line[i]))
    {
        if (isEndOfLine(line[i])) return;
        i++;
    }
}

static bool
getValue(const char *line, int &i, const char *key, char *&returnstring)
{
    const unsigned int length = strlen(key);
    if (strncmp(line + i, key, length) == 0)
    {
        i += length;
        int istart = i;
        skipPastToken(i, line);
        returnstring = new char[i - istart + 1];
        strncpy(returnstring, (line + istart), i - istart);
        returnstring[i-istart] = '\0';
        return(true);
    }
    return(false);
}

// This routine returns the next token in the line and its type.
int 
parse(int &i, const char *line, char *&returnstring)
{
    if (i >= (int) strlen(line)) return(ENDOFLINE);

    if (returnstring != NULL) cerr << "returnstring is not NULL!\n";

    int returnval = UNKNOWN;

    if (isDelimiter(line[i]))
        returnval = DELIMITER;
    else if (isEndOfLine(line[i]))
        returnval = ENDOFLINE;
    else if (getValue(line, i, "align=", returnstring))
        returnval = ALIGN;
    else if (getValue(line, i, "altcirc=", returnstring))
        returnval = ALTCIRC;
    else if (getValue(line, i, "color=", returnstring))
        returnval = COLOR;
    else if (getValue(line, i, "font=", returnstring))
        returnval = FONT;
    else if (getValue(line, i, "fontsize=", returnstring))
        returnval = FONTSIZE;
    else if (getValue(line, i, "image=", returnstring))
        returnval = IMAGE;
    else if (getValue(line, i, "symbolsize=", returnstring))
	returnval = SYMBOLSIZE;
    else if (getValue(line, i, "position=", returnstring))
        returnval = POSITION;
    else if (getValue(line, i, "radius=", returnstring))
        returnval = RADIUS;
    else if (getValue(line, i, "spacing=", returnstring))
        returnval = SPACING;
    else if (line[i] == '"')
    {
        int istart = ++i;
        while (line[i] != '"') 
        {
            i++;
            if (i >= (int) strlen(line))
            {
                cerr << "Unterminated string in marker file!\n";
                return(UNKNOWN);
            }
        }
        returnstring = new char[i - istart + 1];
        strncpy(returnstring, (line + istart), i - istart);
        returnstring[i-istart] = '\0';
        returnval = NAME;
    }
    else if (line[i] == '{')
    {
        int istart = ++i;
        while (line[i] != '}') 
        {
            i++;
            if (i >= (int) strlen(line))
            {
                cerr << "Unterminated string in marker file!\n";
                return(UNKNOWN);
            }
        }
        returnstring = new char[i - istart + 1];
        strncpy(returnstring, (line + istart), i - istart);
        returnstring[i-istart] = '\0';
        returnval = NAME;
    }
    else if (strncmp(line+i, "timezone=", 9) == 0)
    {
        i += 9;
        int istart = i;
        while (line[i] == '/' || line[i] == ',' || !isDelimiter(line[i]))
        {
            if (isEndOfLine(line[i])) break;
            i++;
        }
        returnstring = new char[i - istart + 1];
        strncpy(returnstring, (line + istart), i - istart);
        returnstring[i-istart] = '\0';
        returnval = TIMEZONE;
    }
    else if (strncmp(line+i, "trail={", 7) == 0)
    {
        i += 7;
        int istart = i;
        while (line[i] != '}') 
        {
            i++;
            if (i >= (int) strlen(line))
            {
                cerr << "Unterminated trail value in satellite file!\n";
                return(UNKNOWN);
            }
        }
        returnstring = new char[i - istart + 1];
        strncpy(returnstring, (line + istart), i - istart);
        returnstring[i-istart] = '\0';
        returnval = TRAIL;
    }
    else if (strncmp(line+i, "transparent={", 13) == 0)
    {
        i += 13;
        int istart = i;
        while (line[i] != '}') 
        {
            i++;
            if (i >= (int) strlen(line))
            {
                cerr << "Unterminated pixel value in marker file!\n";
                return(UNKNOWN);
            }
        }
        returnstring = new char[i - istart + 1];
        strncpy(returnstring, (line + istart), i - istart);
        returnstring[i-istart] = '\0';
        returnval = TRANSPARENT;
    }
    else // assume it's a latitude/longitude value
    {
        int istart = i;
        skipPastToken(i, line);
        returnstring = new char[i - istart + 1];
        strncpy(returnstring, (line + istart), i - istart);
        returnstring[i-istart] = '\0';

        double temp;
        sscanf(returnstring, "%lf", &temp);
        if (temp > -360 && temp < 360)
        {
            returnval = LATLON; // else defaults to UNKNOWN
        }
    }
#if 0
    if (opts.debug)
    {
        string returntype;
        switch (returnval)
        {
        case UNKNOWN:
            returntype = "UNKNOWN";
            break;
        case DELIMITER:
            returntype = "DELIMITER";
            break;
        case ENDOFLINE:
            returntype = "ENDOFLINE";
            break;
        case ALIGN:
            returntype = "ALIGN";
            break;
        case COLOR:
            returntype = "COLOR";
            break;
        case FONT:
            returntype = "FONT";
            break;
        case FONTSIZE:
            returntype = "FONTSIZE";
            break;
        case IMAGE:
            returntype = "IMAGE";
            break;
        case POSITION:
            returntype = "POSITION";
            break;
        case SPACING:
            returntype = "SPACING";
            break;
        case NAME:
            returntype = "NAME";
            break;
        case TIMEZONE:
            returntype = "TIMEZONE";
        case TRANSPARENT:
            returntype = "TRANSPARENT";
            break;
        case LATLON:
            returntype = "LATLON";
            break;
        default:
            returntype = "?";
        }

        cout << "token is " 
             << (returnstring == NULL ? "NULL" : returnstring)
             << "\t(" << returntype << ")"
             << endl;
    }
#endif
    return(returnval);
}
