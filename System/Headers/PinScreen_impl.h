#include <osg/BoundingBox>

inline PinScreen::PinScreen()
:width(0), height(0), inited(false),group(new osg::Group()),position_pin_screen(new osg::MatrixTransform())
,horizontal_spacing(0.0f), vertical_spacing(0.0f), lod_levels(0), division_number(2), lod_distance(100.0f),max_movement(0)
{

	this->position_pin_screen->addChild(group.get());

	//createSpecularHighlights();
}
inline PinScreen::~PinScreen()
{
	free();
}
inline void PinScreen::free()
{
	if(inited)
		delete[] pins;
}
inline void PinScreen::setSize(int width, int height)
{
	this->width = width;
	this->height = height;
}
inline int PinScreen::getWidth() const
{
	return width;
}
inline int PinScreen::getHeight() const
{
	return height;
}
inline osg::Node* PinScreen::getNode()
{
	return position_pin_screen.get();
}
inline float PinScreen::maxMovement() const
{
	return max_movement;
}
inline void PinScreen::overrideMaxMovement(float const max_movement)
{
	this->max_movement = max_movement;
}
inline osg::MatrixTransform* PinScreen::getPin(int const x, int const y)
{
	return pins[y * width + x].get();
}
inline float PinScreen::getHorizontalSpacing()
{
	return horizontal_spacing;
}
inline float PinScreen::getVerticalSpacing()
{
	return vertical_spacing;
}
inline int PinScreen::lodLevel() const
{
	return lod_levels;
}
inline void PinScreen::setLod(int const lod_levels) 
{
	this->lod_levels = lod_levels;
}

inline int PinScreen::getDivisionNumber() const
{
	return division_number;
}
inline void PinScreen::setDivisionNumber(int const divisions)
{
	division_number = divisions;
}
inline float PinScreen::getLodDistance() const
{
	return lod_distance;
}
inline void PinScreen::setLodDistance(float const lod_distance)
{
	this->lod_distance = lod_distance;
}
inline osg::BoundingBox PinScreen::getPinBoundingBox() const
{
	return pin_bounding_box;
}
inline float PinScreen::horizontalPinSize() const
{
	return (pin_bounding_box.xMax() - pin_bounding_box.xMin());	
}
inline float PinScreen::verticalPinSize() const
{
	return (pin_bounding_box.zMax() - pin_bounding_box.zMin());
}
inline PinScreen::PinScreenDeleteVisitor::PinScreenDeleteVisitor()
:osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
}
inline PinScreen::PinBoundingBoxGeneratorVisitor::PinBoundingBoxGeneratorVisitor()
:osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
}
inline osg::BoundingBox PinScreen::PinBoundingBoxGeneratorVisitor::getBoundingBox() const
{
	return bounding_box;
}