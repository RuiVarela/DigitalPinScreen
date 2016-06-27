#include "../Headers/PinScreen.h"
#include "../Headers/GfxException.h"

#include <osg/TextureCubeMap>
#include <osg/TexGen>
#include <osg/TexEnvCombine>
#include <osgUtil/ReflectionMapGenerator>
#include <osgUtil/HighlightMapGenerator>
#include <osgUtil/HalfWayMapGenerator>

#include <osg/Geometry>
#include <osg/LOD>

#include <iostream>
#include <string>

void PinScreen::update()
{
	// update all lod levels
	if (lod_levels)
	{
		OsgNodeList::iterator i = lod_nodes.begin();

		while(i != lod_nodes.end())
		{
			osg::Vec3 scale = (*i)->asGroup()->getChild(1)->asTransform()->asMatrixTransform()->getMatrix().getScale();
			osg::Vec3 center = (*i)->asGroup()->getChild(0)->getBound().center();

			(*i)->asGroup()->getChild(1)->asTransform()->asMatrixTransform()->setMatrix(
				osg::Matrix::scale(scale)
				* osg::Matrix::translate(center)
				);

			++i;
		}
	}
}

void PinScreen::PinBoundingBoxGeneratorVisitor::apply(osg::Geode &node)
{
	traverse(node);

	//cycle drawables
	osg::Geode::DrawableList const &drawable_list = node.getDrawableList();
	osg::Geode::DrawableList::const_iterator i = drawable_list.begin();
	while (i != drawable_list.end())
	{
		if ( (*i).get()->asGeometry() )
		{
			//cycle vertex
			osg::Vec3Array *vertex = (osg::Vec3Array*) (*i).get()->asGeometry()->getVertexArray();
			osg::Vec3Array::iterator::const_iterator v = vertex->begin();

			while (v != vertex->end())
			{
				bounding_box.expandBy(*v);
				++v;
			}	
		}
		++i;
	}
}
void PinScreen::PinScreenDeleteVisitor::apply(osg::Group &node)
{
	if (node.getName() != "Pin_Transform")
	{
		traverse(node);
		list_of_groups.push_back(&node);
	}
}
void PinScreen::PinScreenDeleteVisitor::destroy()
{
	std::list<osg::Group*>::iterator i = list_of_groups.begin();

	while(i != list_of_groups.end())
	{
		int children = (*i)->asGroup()->getNumChildren();
		if (children == 0)
			children = 1;

		(*i)->asGroup()->removeChildren (0, children - 1);

		i = list_of_groups.erase(i);

	}
}
osg::Node* PinScreen::bottomUpDivider(OsgNodeTable node_table, int const number_of_divisions)
{
	osg::Node* out_group = 0;

	int level = 0;

	while (node_table.size() > 1)
	{
		bool y_is_dividable = ( (float(node_table.size()) / float(number_of_divisions)) - int(node_table.size() / number_of_divisions) ) == 0;
		bool x_is_dividable = ( (float(node_table[0].size()) / float(number_of_divisions)) - int(node_table[0].size() / number_of_divisions) ) == 0;

			//	std::cout << "Is Dividable (x,y): " << x_is_dividable << " " << y_is_dividable << std::endl;

		int n_lines_groups = ceil( float(node_table.size()) / float(number_of_divisions) );
		int n_cols_groups = ceil( float(node_table[0].size()) / float(number_of_divisions) );

			//	std::cout << "Number Of Groups (lines,cols): " << n_lines_groups << " " << n_cols_groups << std::endl;

		int last_line_number_of_nodes = ( (float(node_table.size()) / float(number_of_divisions)) - int(node_table.size() / number_of_divisions) ) * number_of_divisions;
		int last_col_number_of_nodes = ( (float(node_table[0].size()) / float(number_of_divisions)) - int(node_table[0].size() / number_of_divisions) ) * number_of_divisions;

		//	std::cout << "Last (lines,cols) Number Of Nodes: " << last_line_number_of_nodes << " " << last_col_number_of_nodes << std::endl;

		//		out_group = new osg::Group();

		OsgNodeTable next_level_table;

		for (int line_group = 0; line_group != n_lines_groups; ++line_group)
		{
			next_level_table.push_back(OsgNodeList());

			for (int col_group = 0; col_group != n_cols_groups; ++col_group)
			{
				bool is_left_over = (!x_is_dividable && (col_group == (n_cols_groups -1)) ) || 
					(!y_is_dividable && (line_group == (n_lines_groups -1)) ) ;

				osg::Group* new_group = new osg::Group();

				// group limits on the node table;
				int line_start = line_group * number_of_divisions;
				int line_end = line_start + number_of_divisions;

				// ajust limits if last line
				if (!y_is_dividable && (line_group == (n_lines_groups -1)) )
				{
					line_end = line_start + last_line_number_of_nodes;
				}

				int col_start = col_group * number_of_divisions;
				int col_end = col_start + number_of_divisions;

				// ajust limits if last col
				if (!x_is_dividable && (col_group == (n_cols_groups -1)) )
				{
					col_end = col_start + last_col_number_of_nodes;
				}

				//		std::cout << "Group (line, col): " << line_group << " " << col_group<< std::endl;
				//		std::cout << "Start (line, col): " << line_start << " " << col_start << std::endl;
				//		std::cout << "End (line, col): " << line_end << " " << col_end << std::endl;

				for (int l = line_start; l != line_end; ++l)
				{
					for (int c = col_start; c != col_end; ++c)
					{
						new_group->addChild( node_table[l][c] );

						if (node_table[l][c]->getName() == "LeftOverGroup")
						{
							is_left_over = true;
						}
					}
				}

				// Identify leftOverGroup
				if (is_left_over)
				{
					new_group->setName("LeftOverGroup");
				}

				// Create a Lod Node
				if (!is_left_over && lod_levels && (level < lod_levels))
				{
					int x_elements = pow(number_of_divisions, (level + 1));
					int z_elements = x_elements;

					float x_scale_factor = (x_elements  * horizontalPinSize() + (x_elements  - 1) * horizontal_spacing) / horizontalPinSize();
					float z_scale_factor = (z_elements  * verticalPinSize() + (z_elements  - 1) * vertical_spacing) / verticalPinSize();

					osg::MatrixTransform *lod_distant = new osg::MatrixTransform(
						osg::Matrix::scale(x_scale_factor , 1.0f, z_scale_factor)
						* osg::Matrix::translate( new_group->getBound().center() )
						);

					lod_distant->addChild(pin.get());

					osg::LOD * lod_node = new osg::LOD();

					lod_node->setName("Pin_Transform"); // for correct deletion

					float near = (level + 1) * lod_distance;
					float far = (level + 1) * lod_distance + lod_distance;

					lod_node->addChild(new_group, 0 , near);
					lod_node->addChild(lod_distant, near, far);
					lod_node->addChild(lod_distant, far, 100000000000.0f);

					lod_nodes.push_back(lod_node);
					next_level_table.back().push_back(lod_node);
				}
				else
				{
					next_level_table.back().push_back(new_group);
				}

				//	std::cout << "Group Children : " << new_group->getNumChildren() << std::endl;				
			}
		}
		node_table = next_level_table;
		++level;
	}
	if (node_table.size() == 1)
		out_group = node_table[0][0];

	return out_group;
}
void PinScreen::clearPinScreen()
{
	if (inited)
	{

		lod_nodes.clear();
		PinScreenDeleteVisitor delete_all;
		group->accept(delete_all);
		delete_all.destroy();

		while (group->getNumChildren() >0)
			group->removeChild( group->getChild(0) ) ;

		free();
		inited = false;
	}
}
void PinScreen::buildPinScreen()
{
	// build a pin matrix
	pins = new osg::ref_ptr<osg::MatrixTransform>[width * height];

	osg::ref_ptr<osg::MatrixTransform> *current_pin = &pins[0];

	OsgNodeTable table;
	// bulid the transform array

	for (int current_line = 0; current_line != height; ++current_line)
	{
		table.push_back(OsgNodeList()); // new line

		for (int current_col = 0; current_col != width; ++current_col)
		{
			// set the initial position
			*current_pin = new osg::MatrixTransform(osg::Matrix::translate(
				(horizontalPinSize() + horizontal_spacing) * current_col, 0.0f ,
				(verticalPinSize() + vertical_spacing) * current_line)
				);

			current_pin->get()->setName("Pin_Transform");

			current_pin->get()->addChild(pin.get());
			table.back().push_back(current_pin->get());

			++current_pin; 
		}
	}

	group->addChild(bottomUpDivider(table, division_number));

	position_pin_screen->setMatrix(osg::Matrix::translate( -group->getBound().center().x(), 0.0f, -group->getBound().center().z()) );
	max_movement = (pin.get()->getBound().radius() * 2.0f);
	inited = true;
}
void PinScreen::loadPin(std::string const &file_name)
{
	// remove all pins
	if (inited)
	{
		osg::ref_ptr<osg::MatrixTransform> *current_pin = &pins[0];
		int size = width * height;
		for (int i = 0; i != size; ++i)
		{
			current_pin->get()->removeChild(pin.get());
			++current_pin;
		}
	}
	pin = osgDB::readNodeFile(file_name);

	//calculate bounding_box
	PinBoundingBoxGeneratorVisitor bouding_box_generator;
	pin->accept(bouding_box_generator);

	pin_bounding_box = bouding_box_generator.getBoundingBox();

	/*
	std::cout << "xMin: " << pin_bounding_box.xMin() << " "
	<< "xMax: " << pin_bounding_box.xMax() << " "
	<< "yMin: " << pin_bounding_box.yMin() << " "
	<< "yMax: " << pin_bounding_box.yMax() << " "
	<< "zMax: " << pin_bounding_box.zMin() << " "
	<< "zMax: " << pin_bounding_box.zMax() << " "
	<< std::endl;
	*/
	// add all pins
	if (inited)
	{
		osg::ref_ptr<osg::MatrixTransform> *current_pin = &pins[0];
		int size = width * height;
		for (int i = 0; i != size; ++i)
		{
			current_pin->get()->addChild(pin.get());
			++current_pin;
		}
	}
}
void PinScreen::setHorizontalSpacing(float const spacing)
{
	if (inited)
	{
		osg::ref_ptr<osg::MatrixTransform> *current_pin = &pins[0];
		for (int current_line = 0; current_line != height; ++current_line)
			for (int current_col = 0; current_col != width; ++current_col)
			{
				osg::Matrix current_matrix = current_pin->get()->asMatrixTransform()->getMatrix();
				osg::Matrix translation = osg::Matrix::translate(-(horizontal_spacing + horizontalPinSize()) * current_col + (spacing + horizontalPinSize()) * current_col, 0.0 , 0.0);
				current_pin->get()->asMatrixTransform()->setMatrix(current_matrix * translation);
				++current_pin;
			}

			position_pin_screen->setMatrix(osg::Matrix::translate(-group->computeBound().center().x(), 0.0f, -group->computeBound().center().z()) );	
	}
	horizontal_spacing = spacing;
}
void PinScreen::setVerticalSpacing(float const spacing)
{
	if (inited)
	{
		osg::ref_ptr<osg::MatrixTransform> *current_pin = &pins[0];
		for (int current_line = 0; current_line != height; ++current_line)
			for (int current_col = 0; current_col != width; ++current_col)
			{
				osg::Matrix current_matrix = current_pin->get()->asMatrixTransform()->getMatrix();
				osg::Matrix translation = osg::Matrix::translate(0.0, 0.0 , -(vertical_spacing + verticalPinSize()) * current_line + (spacing + verticalPinSize()) * current_line);
				current_pin->get()->asMatrixTransform()->setMatrix(current_matrix * translation);
				++current_pin;
			}

			position_pin_screen->setMatrix(osg::Matrix::translate(-group->computeBound().center().x(), 0.0f, -group->computeBound().center().z()) );		
	}
	vertical_spacing = spacing;
}
void PinScreen::createSpecularHighlights()
{

	/*Open Scene Graph CubeMap Example */

	osg::StateSet *ss = group->getOrCreateStateSet();

	// create and setup the texture object
	osg::TextureCubeMap *tcm = new osg::TextureCubeMap;
	tcm->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP);
	tcm->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP);
	tcm->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP);
	tcm->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	tcm->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);    

	// generate the six highlight map images (light direction = [1, 1, -1])
	osgUtil::HighlightMapGenerator *mapgen = new osgUtil::HighlightMapGenerator(
		osg::Vec3(1, 1, -1),            // light direction
		osg::Vec4(1, 0.9f, 0.8f, 1),    // light color
		8);                             // specular exponent

	mapgen->generateMap();

	// assign the six images to the texture object
	tcm->setImage(osg::TextureCubeMap::POSITIVE_X, mapgen->getImage(osg::TextureCubeMap::POSITIVE_X));
	tcm->setImage(osg::TextureCubeMap::NEGATIVE_X, mapgen->getImage(osg::TextureCubeMap::NEGATIVE_X));
	tcm->setImage(osg::TextureCubeMap::POSITIVE_Y, mapgen->getImage(osg::TextureCubeMap::POSITIVE_Y));
	tcm->setImage(osg::TextureCubeMap::NEGATIVE_Y, mapgen->getImage(osg::TextureCubeMap::NEGATIVE_Y));
	tcm->setImage(osg::TextureCubeMap::POSITIVE_Z, mapgen->getImage(osg::TextureCubeMap::POSITIVE_Z));
	tcm->setImage(osg::TextureCubeMap::NEGATIVE_Z, mapgen->getImage(osg::TextureCubeMap::NEGATIVE_Z));

	// enable texturing, replacing any textures in the subgraphs
	ss->setTextureAttributeAndModes(0, tcm, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

	// texture coordinate generation
	osg::TexGen *tg = new osg::TexGen;
	tg->setMode(osg::TexGen::REFLECTION_MAP);
	ss->setTextureAttributeAndModes(0, tg, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

	// use TexEnvCombine to add the highlights to the original lighting
	osg::TexEnvCombine *te = new osg::TexEnvCombine;    
	te->setCombine_RGB(osg::TexEnvCombine::ADD);
	te->setSource0_RGB(osg::TexEnvCombine::TEXTURE);
	te->setOperand0_RGB(osg::TexEnvCombine::SRC_COLOR);
	te->setSource1_RGB(osg::TexEnvCombine::PRIMARY_COLOR);
	te->setOperand1_RGB(osg::TexEnvCombine::SRC_COLOR);
	ss->setTextureAttributeAndModes(0, te, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
}