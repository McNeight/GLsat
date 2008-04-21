#include <iostream>
#include "color.h"
#include "image.h"

using namespace std;

Image::Image()
{
	width = DefaultWidth;
	height = DefaultHeight;
	depth = DefaultDepth;

	pixels = new Color[height * width];
}

Image::Image(Color bg)
{
	width = DefaultWidth;
	height = DefaultHeight;
	depth = DefaultDepth;

//	pixels = new Color[height * width](bg);
}

Image::Image(int w, int h)
{
	width = w;
	height = h;
	depth = DefaultDepth;

	pixels = new Color[height * width];
}

Image::Image(const Image& img)
{
	width = img.width;
	height = img.height;
	depth = img.depth;

	pixels = new Color[height * width];

	for (int i = 0; i < (height * width); i++)
	{
		pixels[i].red(img.pixels[i].red());
		pixels[i].green(img.pixels[i].green());
		pixels[i].blue(img.pixels[i].blue());
	}
}

Image::~Image()
{
	delete [] pixels;
}

void Image::resize(int w, int h)
{
	if ((w > 0 && h > 0) && (w != width || h != height))
	{
		width = w;
		height = h;

		delete [] pixels;
		pixels = new Color[height * width];
	}
}

void Image::setdepth(int d)
{
	if (d > 0)
	{
		depth = d;
	}
}

int Image::getdepth() const
{
	return depth;
}

int Image::getwidth() const
{
	return width;
}

int Image::getheight() const
{
	return height;
}

Color& Image::operator() (int x, int y)
{
	return pixels[(width * y) + x];
}

const Color& Image::operator() (int x, int y) const
{
	return pixels[(width * y) + x];
}

Image& Image::operator= (Image& img)
{
	if (this != &img)
	{
		width = img.width;
		height = img.height;
		depth = img.depth;

		delete [] pixels;
		pixels = new Color[height * width];

		for (int i = 0; i < (height * width); i++)
		{
			pixels[i].red(img.pixels[i].red());
			pixels[i].green(img.pixels[i].green());
			pixels[i].blue(img.pixels[i].blue());
		}		
	}

	return *this;
}

void Image::clear(Color bg)
{
  for (int i = 0; i < (height * width); i++)
	{
		pixels[i].red(bg.red());
		pixels[i].green(bg.green());
		pixels[i].blue(bg.blue());
	}	
}

ostream& operator<<(ostream& os, const Image& image)
{
	Image tempImage(image.getwidth(), image.getheight());

	tempImage.setdepth(image.getdepth());

	os << "P3" << endl;
	os << image.getwidth() << " " << image.getheight() << endl;
  os << image.getdepth() << endl;

	for (int i = 0; i < image.getheight(); i++)
	{
		for (int j = 0; j < image.getwidth(); j++)
		{
			tempImage(j,i) = image(j,i);

			if (tempImage(j,i).red() < 0)
			{
				tempImage(j,i).red(0);
			}
			else if (tempImage(j,i).red() > tempImage.getdepth())
			{
				tempImage(j,i).red(tempImage.getdepth());
			}

			if (tempImage(j,i).green() < 0)
			{
				tempImage(j,i).green(0);
			}
			else if (tempImage(j,i).green() > tempImage.getdepth())
			{
				tempImage(j,i).green(tempImage.getdepth());
			}

			if (tempImage(j,i).blue() < 0)
			{
				tempImage(j,i).blue(0);
			}
			else if (tempImage(j,i).blue() > tempImage.getdepth())
			{
				tempImage(j,i).blue(tempImage.getdepth());
			}
			
			os << tempImage(j,i);
		}

		os << endl;
	}

	return os;
}

istream& operator>>(istream& is, Image& image)
{
	string format;
	int width = 0, height = 0, depth = 0;
	bool valid = true;

	Image tempImage;

	is >> format;
	if (!is)
	{
		valid = false;
	}
	else if (format != "P3")
	{
		is.clear(ios::failbit);
		valid = false;
	}

	if (valid)
	{
		is >> width;

		if (!is)
		{
			char comment;
			is.clear();
			is.get(comment);
			if (comment == '#')
			{
				is.ignore(200, '\n');
			}
			else
			{
				is.clear(ios::failbit);
				valid = false;
			}
		}
		else if (width < 0)
		{
			is.clear(ios::failbit);
			valid = false;
		}
	}

	if (valid)
	{
		is >> height;
		
		if (!is)
		{
			valid = false;
		}
		else if (height < 0)
		{
			is.clear(ios::failbit);
			valid = false;
		}
	}

	if (valid)
	{
		is >> depth;

		if (!is)
		{
			valid = false;
		}
		else if (depth < 0)
		{
			is.clear(ios::failbit);
			valid = false;
		}
	}

	if (valid)
	{
		tempImage.resize(width, height);
		tempImage.setdepth(depth);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if(is)
				{
					is >> tempImage(j,i);
				}
			}
		}
	}

	if(is)
	{
		image = tempImage;
	}

	return is;
}