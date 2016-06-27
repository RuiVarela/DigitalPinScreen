/**
* OSG Workbench
* Ryven 2006
*
* Quake alike Console
**/

#ifndef __CONSOLE__
#define __CONSOLE__

#include <osg/Group>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osgProducer/Viewer>

#include <string>

struct ICommandHandler
{
  virtual ~ICommandHandler() {};
  virtual void handleCommand(char *data) = 0;
};

class Console
{
public:
	enum State 
	{
		uninitialized,
		initialized,
		closed,
		open,
		opening,
		closing
	};
	~Console();
	Console();
	Console(osg::Group *group, int const screen_width, int const screen_height,
		std::string const& background_image_filename,
		std::string const& foreground_image_filename);

	void setGroup(osg::Group *group);
	void setBackgroungImageFilename(std::string const& background_image_filename);
	void setForegroungImageFilename(std::string const& foreground_image_filename);
	void setScreenWidth(int const width);
	void setScreenHeight(int const height);
	int screenWidth() const;
	int screenHeight() const;

	State getState() const;
	void toggleConsole();
	void setConsoleLineNumber(int const lines);
	void setCharSize(float const char_size);
	void setFont(std::string const &filename);

	// Events
	void handleKeyInput(const int input);

	// input buffer
	int nextChar() const;
	int prevChar() const;
	void moveRight();
	void moveLeft();
	void removeChar();
	void removePrevChar();
	void setChar(const char ch);
	void clearInputBuffer();

	//Message buffer
	void clearMessageBuffer();
	int nextMesssage() const;
	void moveToNextMessage();
	void addMessage(char *const message);
	void addMessage(const std::string &message);

	void pageUp();
	void pageDown();

	  // Command Buffer
    void getPrvCommand();
    void getNextCommand();
    void addCommand();

	// Command Handler
	void setCommandHandler(ICommandHandler *const command_handler);
private:
	int screen_width;
	int screen_height;
	std::string background_image_filename;
	std::string foreground_image_filename;
	std::string font_file_name;

	osg::Geometry* buildConsoleGeometry(float const alpha) const;
	void initText();
	void buildConsole();
	void execute();

	osg::ref_ptr<osg::Group> group;
	osg::ref_ptr<osg::Group> hud_group;
	State current_state;
	osg::MatrixTransform* console_open_transformation;

	osg::ref_ptr<osg::Projection> projection;

	float char_size; // char height
	int line_char_n; // number of chars per line
	int lines_n;	// number of lines in the message buffer
	char *lines; 
	int visible_lines; // number of visible ines
	int current_message; //current message in the message buffer
	int visible_index; 

	char *commands ; //CommandBuffer
    int visible_command;
    int current_command;
    int number_of_commands;

	/* Input buffer */
	char *input_buffer;
	int current_char;
	
	/* OSG text */
	osgText::Text* input_buffer_text;
	osgText::Text* current_char_text; 
	osgText::Text* message_buffer_text; 

	void refreshCurrentCharText();
	void refreshInputBufferText();
	void refreshMessageBufferText();

	ICommandHandler *command_handler;

	/*
	* Callback that performs the panning in the background
	*/
	class TexturePanCallback : public osg::Drawable::UpdateCallback
	{
	public:
		TexturePanCallback(double delay = 0.05);
		virtual void update(osg::NodeVisitor* nv, osg::Drawable* drawable);
	private:
		float _phase_s, _phase_t, _phase_scale;
		double _delay, _prev_time;
	};

	/*
	* Callback that controls the window openning
	*/
	class ConsoleStateCallback : public osg::NodeCallback
	{
	public:
		ConsoleStateCallback(Console* console, double const base_time = 0.5);
		void operator()(osg::Node* node, osg::NodeVisitor* node_visitor);
	private:
		Console *console;
		double base_time, start_time;
		bool started;
	};
};

#include "Console_impl.h"

#endif //__CONSOLE__
