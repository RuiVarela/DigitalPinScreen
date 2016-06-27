#ifndef __BASE_SYSTEM_HANDLERS__
#define __BASE_SYSTEM_HANDLERS__

#include "StringToken.h"
#include "System.h"
#include "SkyDome.h"
#include "PinScreen.h"
#include "Console.h"
#include "Drawable.h"
#include "Animation.h"

#include <string>

class ConsoleKeybordEventHandler : public osgGA::GUIEventHandler
{
public:
	ConsoleKeybordEventHandler(Console * console);
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&);
private:
	Console *_console;
};

class SkyDomeHandler : public ISystemVariable
{
public:
	~SkyDomeHandler();
	SkyDomeHandler(System *system, std::string const& base_dir, std::string const& sky_dome_name);
	SkyDomeHandler(System *system, SkyDome *skydome,std::string const& base_dir);
	SkyDome* getSkyDome() const;
	bool handleSystemVariable(SystemVariable *const system_variable, char *parameters);
private:
	SystemVariable world;
	SkyDome *skydome;
	System *system;
	std::string base_dir;
	bool is_owner;
};

class ScriptHandler : public ISystemCommand
{
public:
	~ScriptHandler();
	ScriptHandler(System *system, std::string const& base_dir);
	void handleSystemCommand(int const id, char *parameters);
private:
	System *system;
	std::string base_dir;
};

class PinScreenHandler : public ISystemVariable, ISystemCommand
{
public:
	enum BuiltInCommands
	{
		rebuild_pin_screen,
		draw_pin,
		draw_circle,
		draw_line,
		load_luminance_image,
		play,
		stop
	};
	~PinScreenHandler();
	PinScreenHandler(System *system, std::string const& base__model_dir, std::string const& pin_name, 
		int const horizontal_size, int const vertical_size,
		float const horizontal_spacing, float const vertical_spacing); 
	PinScreen &getPinScreen();

	bool handleSystemVariable(SystemVariable *const system_variable, char *parameters);
	void handleSystemCommand(int const id, char *parameters);

	void setImagesDir(std::string images_dir);

	void rebuildPinScreen();

	void drawPin(char *parameters);
	void drawCircle(char *parameters);
	void drawLine(char *parameters);

	void loadLuminanceImage(char *parameters);

	bool setDrawMode(char *parameters);

	void setAnimationBaseDir(std::string const &base_dir);
	void playAnimation(char *parameters);
	void stopAnimation();
private:
	PinScreen pin_screen;
	std::string base_dir;
	SystemVariable pin_name;
	SystemVariable h_size;
	SystemVariable v_size;
	SystemVariable h_spacing;
	SystemVariable v_spacing;
	System* system;
	std::string images_dir;

	SystemVariable draw_mode;
	SystemVariable draw_linear_duration;
	DrawMode* draw;
	osg::Group* animator;

	SystemVariable pin_mass;
	SystemVariable spring_stiffness;
	SystemVariable spring_damping;

	SystemVariable ps_lod;
	SystemVariable ps_division;
	SystemVariable ps_lod_level_distance;
	SystemVariable ps_max_movement;

	LuminanceImageSequence* animation;
	osg::Group* animation_group;
	std::string animation_base_dir;
};

#include "BaseSystemHandlers_impl.h"

#endif // __BASE_SYSTEM_HANDLERS__