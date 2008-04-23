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

#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <string>

using namespace std;


// Color class :
//    No error checking of r/g/b values!
//      This means that -10 0 999 is a valid color, and
//      should not be prevented.  The assumption made is 
//      that any color depth could be possible.  We don't
//      want to restrict the validity of colors
//    The user of the class must decide upon what colors
//      are valid, and enforce those decisions. In this
//      case it falls upon the image class to decide.


class Color 
{
 public:

  // Default constructor initializes all values
  //   to 0.
  Color();
  Color(int R,int G,int B);

  // setters
  void red(int val);
  void green(int val);
  void blue(int val);

  // getters
  int red() const;
  int green() const;
  int blue() const;  
  
  // output function (optional, will not be tested by autograder)
  void output(ostream& os) const;
  // input function  (optional, will not be tested by autograder)
  void input(istream& is);
  
 private:
  int r, g, b;

};


//
// The extraction and insertion operators are required.
//

//
//   The insertion operator outputs the red green and
//     blue values seperated by spaces.
//   Example : 0 128 0
//
ostream& operator<<(ostream & os, const Color& p);


//
//   The extraction operator reads in three integers.
//     If any read attempt fails, then the color remains
//     unchanged.  At no point in this function should 
//     the stream state be set or cleared.
//
istream& operator>>(istream & os, Color& p);


// Define a few colors
const Color Black();
const Color Red(255,0,0);
const Color Green(0,255,0);
const Color Blue(0,0,255);
const Color White(255,255,255);

#endif   //  COLOR_H

