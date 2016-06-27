#ifndef __DRAWABLE__
#define __DRAWABLE__

#include <string>

#include "PinScreen.h"

/*
* Draw Modes
*/
class DrawMode
{
public:
	DrawMode();
	virtual ~DrawMode();
	virtual void movePin(int const x, int const y, float value) = 0;
	virtual void startMovingPins();
	virtual void finishMovingPins();
	void setPinScreen(PinScreen *pin_screen);
	PinScreen* getPinScreen();
protected:
	PinScreen *screen;
};

class AbsoluteDrawMode : public DrawMode
{
public:
	AbsoluteDrawMode();
	virtual ~AbsoluteDrawMode();
	virtual void movePin(int const x, int const y, float value);
};

class RelativeDrawMode : public DrawMode
{
public:
	RelativeDrawMode();
	virtual ~RelativeDrawMode();
	virtual void movePin(int const x, int const y, float value);
};

/*
* Drawables
*/

class Drawable
{
public:
	Drawable();
	virtual ~Drawable();
	virtual void draw() = 0;
	void setDrawMode(DrawMode *draw_mode);
protected:
	PinScreen *screen;
	DrawMode *draw_mode; 
};

class Line : public Drawable
{
public:
	Line(int const x_origin, int const y_origin, int const x_dest, int const y_dest, float const depth);
	void set(int const x_origin, int const y_origin, int const x_dest, int const y_dest, float const depth);
	virtual ~Line();
	virtual void draw();
private:
	int x_origin, x_dest, y_origin, y_dest;
	float depth;
};

class Circle : public Drawable
{
public:
	Circle(int const x_origin, int const y_origin, int const radius, float const depth);
	void set(int const x_origin, int const y_origin, int const radius, float const depth);
	virtual ~Circle();
	virtual void draw();
private:
	int x_origin, y_origin, radius;
	float depth;
};

class LuminanceImage : public Drawable
{
public:
	LuminanceImage();
	void loadImage(std::string const &image_filename);
	std::string const& getFileName() const;
	virtual ~LuminanceImage();
	virtual void draw();
private:
	std::string file;
	double *data;
	int width, height;
};

#include "Drawable_impl.h"

#endif //endif