inline Console::~Console()
{
	delete[] input_buffer;
}
inline Console::Console()
:group(0), screen_width(0), screen_height(0), current_state(uninitialized),
lines_n(100),font_file_name("Data/Fonts/monof55.ttf"),char_size(20),command_handler(0)
{
}
inline Console::Console(osg::Group *group, int const screen_width, int const screen_height,
						std::string const& background_image_filename,
						std::string const& foreground_image_filename)
						:lines_n(100),font_file_name("Data/Fonts/monof55.ttf"),char_size(20),command_handler(0)
{
	this->screen_width = screen_width;
	this->screen_height = screen_height;
	this->background_image_filename = background_image_filename;
	this->foreground_image_filename = foreground_image_filename;
	this->group = group;

	buildConsole();
	initText();
	current_state = initialized;
}
inline int Console::screenWidth() const
{
	return screen_width;
}

inline int Console::screenHeight() const
{
	return screen_height;
}
inline Console::TexturePanCallback::TexturePanCallback(double const delay) 
:_phase_s(6.0f), _phase_t(2.0f), _phase_scale(5.0f), _delay(delay), _prev_time(0.0) 
{}

inline void Console::setGroup(osg::Group *group)
{
	this->group = group;
	buildConsole();
	initText();
	current_state = initialized;
}
inline void Console::setBackgroungImageFilename(std::string const& background_image_filename)
{
	this->background_image_filename = background_image_filename;
}
inline void Console::setForegroungImageFilename(std::string const& foreground_image_filename)
{
	this->foreground_image_filename = foreground_image_filename;
}
inline void Console::setScreenWidth(int const width)
{
	this->screen_width = screen_width;
}
inline void Console::setScreenHeight(int const height)
{
	this->screen_height = screen_height;
}
inline Console::ConsoleStateCallback::ConsoleStateCallback(Console *console,double const base_time)
:start_time(0), started(false)
{
	this->base_time = base_time;
	this->console = console;
}
inline Console::State Console::getState() const
{
	return current_state;
}

// Input buffer
inline int Console::nextChar() const
{
	return ((current_char + 1) % (line_char_n));
}
inline int Console::prevChar() const
{
	return ((line_char_n + current_char - 1) % (line_char_n));
}
inline void Console::moveRight()
{
	current_char = nextChar();
	refreshCurrentCharText();
}
inline void Console::moveLeft()
{
	current_char = prevChar();
	refreshCurrentCharText();
}
#include <osg/Projection>

inline void Console::clearInputBuffer()
{
	input_buffer[0] = '\0';
	current_char = 0;
	refreshInputBufferText();
	refreshCurrentCharText();
}
inline void Console::refreshInputBufferText(){
	input_buffer_text->setText(input_buffer);
}
inline void Console::setConsoleLineNumber(int const lines)
{
	this->lines_n = lines;
}
inline void Console::setCharSize(float const char_size)
{
	this->char_size = char_size;
}
inline void Console::setFont(std::string const &filename)
{
	this->font_file_name = filename;
}
inline int Console::nextMesssage() const
{
	return ((current_message + 1) % (lines_n));
}
inline void Console::moveToNextMessage()
{
	current_message = nextMesssage();
}
inline void Console::addMessage(const std::string &message)
{
    char *c_str = new char[message.size() + 1];
    strcpy(c_str,message.c_str());    
    c_str[message.size()] = '\0';

    addMessage(c_str);
  
    delete [] c_str; 
}
inline void Console::setCommandHandler(ICommandHandler *const command_handler)
{
    this->command_handler = command_handler;
}