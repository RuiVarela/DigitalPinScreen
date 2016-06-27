#ifndef __PIN_SCREEN__
#define __PIN_SCREEN__

#include <osgDB/ReadFile>
#include <osg/Group>
#include <osg/MatrixTransform>

#include <string>
#include <list>

class PinScreen
{
public:
	typedef std::vector<osg::Node*> OsgNodeList;
	typedef std::vector<OsgNodeList> OsgNodeTable;

	PinScreen();
	~PinScreen();
	void loadPin(std::string const &file_name);
	void setSize(int const width, int const height);
	void setHorizontalSpacing(float const spacing);
	void setVerticalSpacing(float const spacing);
	float getHorizontalSpacing();
	float getVerticalSpacing();

	
	int getWidth() const;
	int getHeight() const;
	osg::Node* getNode();

	void overrideMaxMovement(float const max_movement);
	float maxMovement() const;

	void buildPinScreen();
	void clearPinScreen();
	osg::MatrixTransform* getPin(int const x, int const y);

	// LOD
	int lodLevel() const;
	void setLod(int const lod_levels);
	int getDivisionNumber() const;
	void setDivisionNumber(int const divisions);
	float getLodDistance() const;
	void setLodDistance(float const lod_distance);

	void update(); // after all pin moving, call this method to update the pin screen

	osg::BoundingBox getPinBoundingBox() const;
	float horizontalPinSize() const;
	float verticalPinSize() const;

	/* Spatial Patitioning */
	osg::Node* bottomUpDivider(OsgNodeTable node_table, int const number_of_divisions);

	class PinScreenDeleteVisitor : public osg::NodeVisitor
	{
	public:
		PinScreenDeleteVisitor();
		virtual void apply(osg::Group &node);	
		void destroy();
	private:
		std::list<osg::Group*> list_of_groups;
	};

	class PinBoundingBoxGeneratorVisitor : public osg::NodeVisitor
	{
	public:
		PinBoundingBoxGeneratorVisitor();
		virtual void apply(osg::Geode &node);	
		osg::BoundingBox getBoundingBox() const;
	private:
		osg::BoundingBox bounding_box;
	};

	// animation for each pin
private:
	void free();
	void createSpecularHighlights();
	osg::ref_ptr<osg::Node> pin;
	osg::ref_ptr<osg::Group> group;
	osg::ref_ptr<osg::MatrixTransform> position_pin_screen;
	int width, height;
	bool inited;
	float horizontal_spacing, vertical_spacing;
	osg::ref_ptr<osg::MatrixTransform> *pins;
	int lod_levels;
	int division_number;
	float lod_distance;
	osg::BoundingBox pin_bounding_box;
	float max_movement;

	OsgNodeList lod_nodes;
};

#include "PinScreen_impl.h"

#endif // __PIN_SCREEN__
