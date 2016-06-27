#include <osg/Geometry>
#include <osg/Projection>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>

#include <osgText/Text>

#include <string>
#include <cmath>

#include "../Headers/Console.h"
#include "../Headers/StringToken.h"
#include "../Headers/GfxException.h"
#include "../Headers/StringToken.h"

void Console::TexturePanCallback::update(osg::NodeVisitor* nv, osg::Drawable* drawable)
{
	if (nv->getFrameStamp()) 
	{
		double curr_time = nv->getFrameStamp()->getReferenceTime();

		if (curr_time - _prev_time > _delay) 
		{
			osg::Geometry* geom = drawable->asGeometry();

			float rad = osg::DegreesToRadians(curr_time);

			// zoom scale (0.2 - 1.0)
			float scale = sin(rad * _phase_scale) * 0.2f + 0.8f;

			// calculate new texture coordinates
			float s, t;
			s = ((sin(rad * _phase_s) + 1) * 0.5f) * 3;
			t = ((sin(rad * _phase_t) + 1) * 0.5f) * 1;

			// set new texture coordinate array
			osg::Vec2Array* texcoords = (osg::Vec2Array*) geom->getTexCoordArray(0);

			float w = 3 * scale, h = 1 * scale;
			(*texcoords)[0].set(s, t + h);
			(*texcoords)[1].set(s, t);
			(*texcoords)[2].set(s + w, t);
			(*texcoords)[3].set(s + w, t+h);

			// record time
			_prev_time = curr_time;
		}
	}
}

osg::Geometry* Console::buildConsoleGeometry(float const alpha) const
{
	osg::Geometry* geometry = new osg::Geometry(); 

	osg::Vec3Array* vertices = new osg::Vec3Array();
	vertices->push_back(osg::Vec3(screenWidth(), screenHeight() , 0.0));
	vertices->push_back(osg::Vec3(screenWidth(), screenHeight() + screenHeight() * 0.5, 0.0));
	vertices->push_back(osg::Vec3(0, screenHeight() + screenHeight() * 0.5, 0.0));
	vertices->push_back(osg::Vec3(0, screenHeight(), 0.0));
	geometry->setVertexArray(vertices);

	osg::Vec4Array* colors = new osg::Vec4Array();
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, alpha));
	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	geometry->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vertices->size()) );

	osg::Vec3Array* normals = new osg::Vec3Array();
	normals->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	geometry->setNormalArray(normals);
	geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

	osg::Vec2Array* tex_coords = new osg::Vec2Array();
	tex_coords->push_back(osg::Vec2 (3.0, 0.0));
	tex_coords->push_back(osg::Vec2 (3.0, 1.0));
	tex_coords->push_back(osg::Vec2 (0.0, 1.0));
	tex_coords->push_back(osg::Vec2 (0.0, 0.0));
	geometry->setTexCoordArray(0, tex_coords);

	return geometry;
}

void Console::buildConsole()
{
	/*
	* HUD
	*/

	hud_group = new osg::Group();

	osg::StateSet* state_set = hud_group->getOrCreateStateSet();
	state_set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	state_set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	/* Assign the HUD to a "high" render bin (in this case, 11). Render bins
	are rendered in order, so this one will be rendered after everything else
	(hence, the HUD gets rendered over everything). */
	state_set->setRenderBinDetails (11, "RenderBin");

	/* The HUD must be in an absolute reference system. So, the HUD will be
	attached to this transform */
	osg::MatrixTransform* model_view_absolute = new osg::MatrixTransform();
	model_view_absolute->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	model_view_absolute->setMatrix(osg::Matrix::identity());

	// We'll also want the HUD to use an orthogonal projection.
	projection = new osg::Projection();
	projection->setMatrix(osg::Matrix::ortho2D (0, screenWidth(), 0, screenHeight()));
	projection->addChild(model_view_absolute);
	projection->setNodeMask(0x00000000);

	/* this transformation is responsible for moving the console up and down */
	console_open_transformation = new osg::MatrixTransform();
	console_open_transformation->addChild(hud_group.get());
	model_view_absolute->addChild(console_open_transformation);

	/*
	* Create 2 Quads for the textures
	*/

	// Background geode
	osg::Geode* geode_back = new osg::Geode();
	hud_group->addChild(geode_back);
	geode_back->addDrawable(buildConsoleGeometry(1.0f));
	osg::StateSet* state_set_back = geode_back->getOrCreateStateSet();

	osg::Image* console_image  = osgDB::readImageFile(background_image_filename);
	if (console_image == 0)
		throw GfxException(1001,"Error loading file: " + background_image_filename);

	osg::Texture2D* console_texture = new osg::Texture2D();
	console_texture->setImage(console_image);
	console_texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	console_texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	console_texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	console_texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);	

	osg::TexEnv* texture_back_env = new osg::TexEnv(osg::TexEnv::REPLACE);
	state_set_back->setTextureAttribute(0, texture_back_env, osg::StateAttribute::ON);
	state_set_back->setTextureAttributeAndModes(0, console_texture, osg::StateAttribute::ON);

	// foreground geode
	osg::Geode* geode_front = new osg::Geode();
	hud_group->addChild(geode_front);	
	geode_front->addDrawable(buildConsoleGeometry(0.3f));

	osg::Image* console_front_image  = osgDB::readImageFile(foreground_image_filename);
	if (console_front_image == 0)
		throw GfxException(1001,"Error loading file: " + foreground_image_filename);

	osg::Texture2D* console_front_texture = new osg::Texture2D();
	console_front_texture->setImage(console_front_image);
	console_front_texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	console_front_texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	console_front_texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	console_front_texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);	

	osg::StateSet* state_set_front = geode_front->getOrCreateStateSet();

	state_set_front->setMode(GL_BLEND,osg::StateAttribute::ON);

	osg::TexEnv* texture_front_env = new osg::TexEnv(osg::TexEnv::REPLACE);
	state_set_front->setTextureAttribute(0, texture_front_env, osg::StateAttribute::ON);
	state_set_front->setTextureAttributeAndModes(0, console_front_texture, osg::StateAttribute::ON);

	geode_front->getDrawable(0)->setDataVariance(osg::Object::DYNAMIC);
	geode_front->getDrawable(0)->setUpdateCallback(new TexturePanCallback());

	// we will be changing the texture mapping later
	geode_front->getDrawable(0)->setUseDisplayList(false);

	// Line
	osg::Geode* line = new osg::Geode();
	osg::Geometry* geometry = new osg::Geometry(); 

	osg::Vec3Array* vertices = new osg::Vec3Array();
	vertices->push_back(osg::Vec3(0, screenHeight(), 0.0));
	vertices->push_back(osg::Vec3(screenWidth(), screenHeight() , 0.0));
	geometry->setVertexArray(vertices);

	osg::Vec4Array* colors = new osg::Vec4Array();
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	geometry->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, vertices->size()) );
	line->addDrawable(geometry);
	hud_group->addChild(line); 

	console_open_transformation->setUpdateCallback(new ConsoleStateCallback(this));

	group->addChild(projection.get());

}
/* falta fazer a transição em estado abrir ->fechar */
void Console::ConsoleStateCallback::operator ()(osg::Node* node, osg::NodeVisitor* node_visitor) {

	if (node_visitor->getFrameStamp()) 
	{
		switch(console->current_state)
		{
		case Console::State::opening :
			{
				if (!started)
				{
					started = true;
					start_time = node_visitor->getFrameStamp()->getReferenceTime();
				}
				double elapsed = node_visitor->getFrameStamp()->getReferenceTime() - start_time;
				double move_factor = elapsed / base_time;
				if (move_factor > 1.0)
				{
					move_factor = 1.0;
					console->current_state = Console::State::open;
					started = false;
				}
				float offset = (console->screen_height >> 1) * move_factor;
				console->console_open_transformation->setMatrix(osg::Matrix::translate(0, - offset,0));
			}
			break;

		case Console::State::closing :
			{
				if (!started)
				{
					started = true;
					start_time = node_visitor->getFrameStamp()->getReferenceTime();
				}
				double elapsed = node_visitor->getFrameStamp()->getReferenceTime() - start_time;
				double move_factor = 1 - (elapsed / base_time);
				if (move_factor < 0.0)
				{
					move_factor = 0.0;
					console->current_state = Console::State::closed;
					console->projection->setNodeMask(0x00000000);
					started = false;
				}
				float offset = (console->screen_height >> 1) * move_factor;
				console->console_open_transformation->setMatrix(osg::Matrix::translate(0, - offset,0));
			}
			break;
		default :
			break;
		}
	}

	traverse(node, node_visitor); 
}
void Console::toggleConsole()
{
	switch (current_state)
	{
	case open :
	case opening :
		current_state = closing;
		break;

	case closed :
	case closing :
	case initialized :
		if(projection->getNodeMask() == 0x00000000)
			projection->setNodeMask(0xFFFFFFFF);
		current_state = opening;
		break;

	default:
		throw GfxException(2001,"Ilegal State : " + current_state);
		break;
	}
}
void Console::initText()
{
	float const aspect_ratio = 2.5;

	/* Input buffer */
	input_buffer_text = new osgText::Text();
	input_buffer_text->setFont(font_file_name);
	input_buffer_text->setPosition(osg::Vec3(0.0, screen_height + 4, 0.0f));
	input_buffer_text->setCharacterSize(char_size,aspect_ratio);

	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(input_buffer_text);
	hud_group->addChild(geode);

	/* current char */
	current_char_text = new osgText::Text(); 
	current_char_text->setFont(font_file_name);
	current_char_text->setPosition(osg::Vec3(0.0, screen_height + 3, 0.0f));
	current_char_text->setCharacterSize(char_size,aspect_ratio);

	geode = new osg::Geode();
	geode->addDrawable(current_char_text);
	hud_group->addChild(geode);

	/* message buffer */
	message_buffer_text = new osgText::Text(); 
	message_buffer_text->setFont(font_file_name);
	message_buffer_text->setPosition(osg::Vec3(0.0, screen_height + screen_height * 0.5 - char_size, 0.0f));
	message_buffer_text->setCharacterSize(char_size,aspect_ratio);

	geode = new osg::Geode();
	geode->addDrawable(message_buffer_text);
	hud_group->addChild(geode);

	line_char_n = screenWidth() / (char_size / (aspect_ratio * 2));
	input_buffer = new char[line_char_n + 1];
	input_buffer[0] = '\0';
	current_char = 0;
	refreshCurrentCharText();


	visible_lines = ((screenHeight() * 0.5) - char_size) / char_size;
	lines = new char[lines_n * (line_char_n + 1)];
	clearMessageBuffer();
	current_message = 0;


	number_of_commands = int(0.1 * lines_n);
	commands = new char[number_of_commands * (line_char_n + 1)];
	visible_command = current_command = 0;

	visible_index = 0;

	for (int i = 0; i != number_of_commands; ++i ) // initialize the commands
		commands[i * (line_char_n + 1)] = '\0';

}
void Console::pageUp()
{
	if (current_message != ((lines_n + visible_index - visible_lines) % (lines_n)))
	{
		bool overlap = (visible_index > current_message) &&
			(( (lines_n + visible_index - visible_lines) % (lines_n)) < current_message);

		if(overlap)
			visible_index = ((current_message + visible_lines) % lines_n);
		else
			visible_index = ((lines_n + visible_index - visible_lines) % (lines_n));
	}	
	refreshMessageBufferText();
}
void Console::pageDown()
{
	if (current_message != visible_index)
	{
		bool overlap = (visible_index < current_message) &&
			(( (visible_index + visible_lines) % (lines_n)) > current_message);

		if(overlap)
			visible_index = current_message;
		else
			visible_index = ((visible_index + visible_lines) % (lines_n));	    
	}
	refreshMessageBufferText();
}
void Console::addMessage(char *const message)
{
	strncpy(&lines[current_message * (line_char_n + 1)], message, line_char_n);    
	visible_index = nextMesssage();
	moveToNextMessage(); 

	if (int(strlen(message)) > line_char_n)
		addMessage(&message[line_char_n]);
	else
		refreshMessageBufferText();
}
void Console::getPrvCommand()
{	
	int prev = ((number_of_commands + visible_command - 1) % (number_of_commands));

	if ((prev != current_command) && (commands[prev * (line_char_n + 1)] != '\0'))
	{
		visible_command = prev;

		strcpy(input_buffer,&commands[prev * (line_char_n + 1)]);
		current_char = strlen(input_buffer);

		//when is the last one
		if (current_char == line_char_n)
			--current_char;
	}
	refreshCurrentCharText();
	refreshInputBufferText();
}
void Console::getNextCommand()
{
	int next = ((visible_command + 1) % (number_of_commands));

	if ((next != current_command) && (commands[next * (line_char_n + 1)] != '\0'))
	{
		visible_command = next;

		strcpy(input_buffer,&commands[next * (line_char_n + 1)]);
		current_char = strlen(input_buffer);

		//when is the last one
		if (current_char == line_char_n)
			--current_char;
	}
	refreshCurrentCharText();
	refreshInputBufferText();
}
void Console::addCommand()
{
	if (input_buffer[0] != '\0')
	{
		strcpy(&commands[current_command * (line_char_n + 1)],input_buffer);
		current_command = ((current_command + 1) % (number_of_commands));
		visible_command = current_command;
	}
}
void Console::clearMessageBuffer()
{
	for (int i = 0; i != lines_n; ++i ) // initialize the messages
	{
		lines[i * (line_char_n + 1)] = '.';
		lines[i * (line_char_n + 1) + 1] = '\0';
	}
	visible_index = current_message = 0;
	refreshMessageBufferText();
}
void Console::refreshMessageBufferText()
{
	char *space_buffer = new char[visible_lines * (line_char_n + 1)];
	char *pointer = &space_buffer[0];

	int delta = visible_index - visible_lines;
	int start = (delta >= 0) ? delta : lines_n + delta;
	int end = visible_index;

	while (start != end) 
	{
		pointer = StringToken::strcpyAndNewLine(pointer, &lines[start * (line_char_n + 1)]);
		++pointer;
		start = (start + 1) % (lines_n);
	}
	*pointer = '\0';

	message_buffer_text->setText(space_buffer);
}
void Console::refreshCurrentCharText() 
{
	char *space_buffer = new char[line_char_n + 1];

	int i = 0;
	for (; i != current_char; ++i)
		space_buffer[i] = ' ';

	space_buffer[i] = '_';
	space_buffer[++i] = '\0';

	current_char_text->setText(space_buffer);
	delete[] space_buffer;
}
void Console::setChar(const char ch)
{
	if (input_buffer[current_char] == '\0')
		input_buffer[current_char + 1] = '\0'; 

	input_buffer[current_char] = ch;

	if (current_char != (line_char_n - 1))
		moveRight();

	refreshInputBufferText();
}
void Console::removeChar()
{
	if (input_buffer[current_char] != '\0')
		strcpy(&input_buffer[current_char], &input_buffer[current_char + 1]); 

	refreshInputBufferText();
}
void Console::removePrevChar()
{
	if(current_char != 0)
		moveLeft();

	removeChar();
}
void Console::execute()
{
	addCommand();
	if(command_handler != 0)
		command_handler->handleCommand(input_buffer);
	//	addMessage(input_buffer);
	clearInputBuffer();
}
void Console::handleKeyInput(const int input)
{

	if (input & 0xFF00)
	{
		switch (input)
		{
		case osgGA::GUIEventAdapter::KEY_Right :
			moveRight();
			break;
		case osgGA::GUIEventAdapter::KEY_Left:
			moveLeft();
			break;
		case osgGA::GUIEventAdapter::KEY_Delete :
			removeChar();
			break;
		case osgGA::GUIEventAdapter::KEY_BackSpace :
			removePrevChar();
			break;
		case osgGA::GUIEventAdapter::KEY_Return :
			execute();
			break;
		case osgGA::GUIEventAdapter::KEY_Up :
			getPrvCommand();
			break;
		case osgGA::GUIEventAdapter::KEY_Down :
			getNextCommand();	
			break;
		case osgGA::GUIEventAdapter::KEY_Page_Down :
			pageDown();	
			break;
		case osgGA::GUIEventAdapter::KEY_Page_Up :
			pageUp();	
			break;
		default:
			break;
		}
	}
	else
	{
		setChar(char(input));
	}
}
