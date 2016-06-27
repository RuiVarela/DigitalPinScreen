#include "../Headers/Drawable.h"
#include "../Headers/GfxException.h"

//#include <iostream>
#include <osg/Image>

void Line::draw()
{

	bool steep = abs(y_dest - y_origin) > abs(x_dest - x_origin);

	if (steep)
	{
		int temp = x_origin;
		x_origin = y_origin;
		y_origin = temp;

		temp = x_dest;
		x_dest = y_dest;
		y_dest = temp;
	}
	if (x_origin > x_dest)
	{
		int temp = x_origin;
		x_origin = x_dest;
		x_dest = temp;

		temp = y_origin;
		y_origin = y_dest;
		y_dest = temp;
	}

	int deltax = x_dest - x_origin;
	int deltay = abs(y_dest - y_origin);
	int error = 0;
	int ystep = 1;
	int y = y_origin;

	if (y_origin >= y_dest)
		ystep = -1;

	for (int x = x_origin; x != x_dest; ++x)
	{
		if (steep)
		{
			draw_mode->movePin(y, x, depth);
		}
		else
		{
			draw_mode->movePin(x, y, depth);
		}

		error = error + deltay;

		if ((error << 1) >= deltax)
		{
			y = y + ystep;
			error = error - deltax;
		}
	}
}
void Circle::draw()
{
	int x = 0;
	int y = radius;
	int p = ( (5 - (radius << 2) ) ) >> 2;

	if (x == 0) 
	{
		draw_mode->movePin( x_origin, y_origin + y, depth);
		draw_mode->movePin( x_origin, y_origin - y, depth);
		draw_mode->movePin( x_origin + y, y_origin, depth);
		draw_mode->movePin( x_origin - y, y_origin, depth);
	} 
	else 
	{
		if (x == y) 
		{
			draw_mode->movePin( x_origin + x, y_origin + y, depth);
			draw_mode->movePin( x_origin - x, y_origin + y, depth);
			draw_mode->movePin( x_origin + x, y_origin - y, depth);
			draw_mode->movePin( x_origin - x, y_origin - y, depth);
		} 
		else 
		{
			if (x < y)
			{
				draw_mode->movePin( x_origin + x, y_origin + y, depth);
				draw_mode->movePin( x_origin - x, y_origin + y, depth);
				draw_mode->movePin( x_origin + x, y_origin - y, depth);
				draw_mode->movePin( x_origin - x, y_origin - y, depth);
				draw_mode->movePin( x_origin + y, y_origin + x, depth);
				draw_mode->movePin( x_origin - y, y_origin + x, depth);
				draw_mode->movePin( x_origin + y, y_origin - x, depth);
				draw_mode->movePin( x_origin - y, y_origin - x, depth);
			}
		}
	}

	while (x < y) {
		x++;
		if (p < 0) {
			p += (x << 1) + 1;
		} else {
			y--;
			p += ((x - y) << 1) + 1;
		}

		if (x == 0) 
		{
			draw_mode->movePin( x_origin, y_origin + y, depth);
			draw_mode->movePin( x_origin, y_origin - y, depth);
			draw_mode->movePin( x_origin + y, y_origin, depth);
			draw_mode->movePin( x_origin - y, y_origin, depth);
		} 
		else 
		{
			if (x == y) 
			{
				draw_mode->movePin( x_origin + x, y_origin + y, depth);
				draw_mode->movePin( x_origin - x, y_origin + y, depth);
				draw_mode->movePin( x_origin + x, y_origin - y, depth);
				draw_mode->movePin( x_origin - x, y_origin - y, depth);
			} 
			else 
			{
				if (x < y)
				{
					draw_mode->movePin( x_origin + x, y_origin + y, depth);
					draw_mode->movePin( x_origin - x, y_origin + y, depth);
					draw_mode->movePin( x_origin + x, y_origin - y, depth);
					draw_mode->movePin( x_origin - x, y_origin - y, depth);
					draw_mode->movePin( x_origin + y, y_origin + x, depth);
					draw_mode->movePin( x_origin - y, y_origin + x, depth);
					draw_mode->movePin( x_origin + y, y_origin - x, depth);
					draw_mode->movePin( x_origin - y, y_origin - x, depth);
				}
			}
		}

	}
}
void LuminanceImage::loadImage(std::string const &image_filename)
{
	if (!screen)
		throw GfxException(6005,"Unable to load LuminanceImage, DrawMode not set");

	if (data)
		delete [] data;

	osg::ref_ptr<osg::Image> image = osgDB::readImageFile(image_filename);

	data = 0;
	width = height = 0;

	if (! image.get() )
		throw GfxException(6001,"Image File not found (LuminanceImage)");

	if (image->getPixelFormat() != GL_RGB)
		throw GfxException(6002,"Invalid Pixel Format for image (only rgb is supported).");

	if (image->getDataType() != GL_UNSIGNED_BYTE)
		throw GfxException(6003,"Invalid Data Type (only unsigned byte is supported).");

	
	file = image_filename;
	width = ( (image->s() < screen->getWidth()) ? image->s() : screen->getWidth() );
	height = ( (image->t() < screen->getHeight()) ? image->t() : screen->getHeight() );
	data = new double[width * height]();

	unsigned char* image_iterator = &image->data()[0];
	double * luminance_iterator = &data[0];

	double max_movement = screen->maxMovement() * 0.8;
	//double const max_luminace = 255 * 0.299 + 255 * 0.587 + 255 * 0.114;
	double const max_luminace = 255 * 0.2126 + 255 * 0.7156 + 255 * 0.0722;
	for (int line = 0; line != height; ++line)
	{
		for (int col = 0; col!= width; ++col)
		{
			unsigned char red = *image_iterator;
			++image_iterator;
			unsigned char green = *image_iterator;
			++image_iterator;
			unsigned char blue = *image_iterator;
			++image_iterator;

			double luminance = red * 0.2126 + green * 0.7156 + blue * 0.0722;

			//invert -> black far, white near
			//luminance = max_luminace - luminance;

			double position = ( ( (max_movement * 2.0) * luminance ) / max_luminace ) - max_movement;

			*luminance_iterator = position;

			//std::cout << *luminance_iterator << std::endl;

			++luminance_iterator;
		}
	}

	//std::cout << " drawable--- image " << "Height: " << height << " Width: " << width << std::endl;
}

void LuminanceImage::draw()
{
	double * luminance_iterator = &data[0];

	for (int line = 0; line != height; ++line)
	{
		for (int col = 0; col!= width; ++col)
		{
			draw_mode->movePin(col, line, *luminance_iterator);

			++luminance_iterator;
		}
	}
}