/*
 *  GLsat: An OpenGL-based 3D satellite tracker
 *  Copyright (C) 2002-2008  Neil McNeight <mcneight@users.sf.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#include <iostream>
#include <string>

#include "color.h"

using namespace std;


/*
 *
 */
Color::Color()
{
	r = g = b = 0;
}


/*
 *
 */
Color::Color(int R, int G, int B)
{
	r = R;
	g = G;
	b = B;
}


/*
 *
 */
void Color::red(int val)
{
	r = val;
}

void Color::green(int val)
{
	g = val;
}

void Color::blue(int val)
{
	b = val;
}


/*
 *
 */
int Color::red() const
{
	return r;
}

int Color::green() const
{
	return g;
}

int Color::blue() const
{
	return b;
}


/*
 *
 */
void Color::output(ostream& os) const
{
	os << r << " " << g << " " << b << " ";
}


/*
 *
 */
void Color::input(istream& is)
{
	int red, green, blue;
	
	is >> red >> green >> blue;

	if (is)
	{
		r = red;
		g = green;
		b = blue;
	}
}


/*
 *
 */
ostream& operator<<(ostream & os, const Color& p)
{
	p.output(os);
	return os;
}


/*
 *
 */
istream& operator>>(istream & is, Color& p)
{
	p.input(is);
	return is;
}
