/* 
* DrawMode
*/
inline DrawMode::DrawMode()
:screen(0)
{
}
inline DrawMode::~DrawMode()
{
}
inline void DrawMode::setPinScreen(PinScreen *pin_screen)
{
	this->screen = pin_screen;
}
inline PinScreen* DrawMode::getPinScreen()
{
	return screen;
}
inline void DrawMode::startMovingPins()
{
}
inline void DrawMode::finishMovingPins()
{
	screen->update();
}
inline AbsoluteDrawMode::AbsoluteDrawMode()
{
}
inline AbsoluteDrawMode::~AbsoluteDrawMode()
{
}
inline void AbsoluteDrawMode::movePin(int const x, int const y, float value)
{
	screen->getPin(x,y)->setMatrix( osg::Matrix::translate( (screen->getHorizontalSpacing() + screen->horizontalPinSize()) * x, value, (screen->getVerticalSpacing() + screen->verticalPinSize()) * y ) );
}
inline RelativeDrawMode::RelativeDrawMode()
{
}
inline RelativeDrawMode::~RelativeDrawMode()
{
}
inline void RelativeDrawMode::movePin(int const x, int const y, float value)
{
	screen->getPin(x,y)->setMatrix( screen->getPin(x, y)->getMatrix() * osg::Matrix::translate(0.0, value, 0.0 ) );
}

/*
* Drawable
*/
inline Drawable::Drawable()
:screen(0), draw_mode(0)
{
	//draw_mode = new AbsoluteDrawMode();
}
inline Drawable::~Drawable()
{
}
inline void Drawable::setDrawMode(DrawMode *draw_mode)
{
	this->draw_mode = draw_mode;
	screen = draw_mode->getPinScreen();
}

/*
 * Line
 */

inline Line::Line(int const x_origin, int const y_origin, int const x_dest, int const y_dest, float const depth)
{
	set(x_origin, y_origin, x_dest, y_dest, depth);
}
inline Line::~Line()
{
}
inline void Line::set(int const x_origin, int const y_origin, int const x_dest, int const y_dest, float const depth)
{
	this->x_origin = x_origin;
	this->x_dest = x_dest;

	this->y_origin = y_origin;
	this->y_dest = y_dest;

	this->depth = depth;
}
/*
* Circle
*/
inline Circle::~Circle()
{
}
inline Circle::Circle(int const x_origin, int const y_origin, int const radius, float const depth)
{
	set(x_origin, y_origin, radius, depth);
}
inline void Circle::set(int const x_origin, int const y_origin, int const radius, float const depth)
{
	this->x_origin = x_origin;
	this->y_origin = y_origin;
	this->radius = radius;
	this->depth = depth;
}
/*
* LuminanceImage
*/
inline LuminanceImage::LuminanceImage()
:data(0)
{
}
inline LuminanceImage::~LuminanceImage()
{
	if(data)
		delete[] data;
}
inline std::string const& LuminanceImage::getFileName() const
{
	return file;
}