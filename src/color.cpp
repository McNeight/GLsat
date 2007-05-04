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
