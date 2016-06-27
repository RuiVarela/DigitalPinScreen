#include "Script.h"

inline ScriptHandler::~ScriptHandler()
{
}
inline ScriptHandler::ScriptHandler(System *system, std::string const& base_dir)
{
		this->base_dir = base_dir;
		this->system = system;
		system->registerCommand("run", 0, this);
}
inline ConsoleKeybordEventHandler::ConsoleKeybordEventHandler(Console * console)
:_console(console)
{
}
inline SkyDomeHandler::SkyDomeHandler(System *system, std::string const& base_dir, std::string const& sky_dome_name)
:world("sky_dome",(char*)sky_dome_name.c_str()), is_owner(true)
{
	this->skydome = new SkyDome(base_dir + sky_dome_name + "\\");
	this->system = system;
	this->base_dir = base_dir;
	system->registerVariable(&world,this);
}

inline SkyDomeHandler::SkyDomeHandler(System *system, SkyDome *skydome,std::string const& base_dir)
:world("sky_dome","default"), is_owner(false)
{
	this->skydome = skydome;
	this->system = system;
	this->base_dir = base_dir;
	system->registerVariable(&world,this);
}
inline SkyDome* SkyDomeHandler::getSkyDome() const
{
	return skydome;
}
inline PinScreenHandler::~PinScreenHandler()
{
	if (animator) // é um animator
	{
		animator = 0;
	}
	else // é um draw mode k n é "Referenced", apagar explicitamente
	{
		delete draw;
	}
}
inline PinScreenHandler::PinScreenHandler(System *system, std::string const& dir, std::string const& pin, 
										  int const horizontal_size, int const vertical_size,
										  float const horizontal_spacing, float const vertical_spacing)
										  :base_dir(dir), pin_name("ps_pin_name",(char*)pin.c_str()),
										  h_size("ps_h_pins",horizontal_size), v_size("ps_v_pins",vertical_size),
										  h_spacing("ps_h_spacing",horizontal_spacing), v_spacing("ps_v_spacing",vertical_spacing),
										  draw_mode("ps_draw_mode","absolute"),
										  draw_linear_duration("ps_linear_duration", 1.0f),
										  pin_mass("ps_pin_mass", 1.0f),
										  spring_stiffness("ps_spring_stiffness", 10.0f),
										  spring_damping("ps_spring_damping", 1.0f),
										  ps_lod("ps_lod", 0),
										  ps_division("ps_division", 2),
										  ps_lod_level_distance("ps_lod_distance", 300.0f),
										  ps_max_movement("ps_max_movement", 0.0f),
										  animation(0)
{
	animator = 0;
	draw = new AbsoluteDrawMode();
	draw->setPinScreen(&pin_screen);

	pin_screen.loadPin(dir + pin);
	pin_screen.setSize(horizontal_size, vertical_size);
	pin_screen.setHorizontalSpacing(horizontal_spacing);
	pin_screen.setVerticalSpacing(vertical_spacing);
	pin_screen.setLod(ps_lod.getInt());
	pin_screen.setLodDistance(ps_lod_level_distance.getFloat());
	pin_screen.setDivisionNumber(ps_division.getInt());
	pin_screen.buildPinScreen();

	system->registerVariable(&pin_name, this);
	system->registerVariable(&h_size);
	system->registerVariable(&v_size);
	system->registerVariable(&h_spacing, this);
	system->registerVariable(&v_spacing, this);
	system->registerVariable(&draw_mode, this);
	system->registerVariable(&draw_linear_duration, this);
	system->registerVariable(&pin_mass);
	system->registerVariable(&spring_stiffness);
	system->registerVariable(&spring_damping);
	system->registerVariable(&ps_lod);
	system->registerVariable(&ps_division);
	system->registerVariable(&ps_lod_level_distance);
	system->registerVariable(&ps_max_movement, this);

	system->registerCommand("rebuildPinScreen", rebuild_pin_screen, this);
	system->registerCommand("drawPin", draw_pin, this);
	system->registerCommand("drawCircle", draw_circle, this);
	system->registerCommand("drawLine", draw_line, this);
	system->registerCommand("loadLuminanceImage", load_luminance_image, this);
	system->registerCommand("play", play, this);
	system->registerCommand("stop", stop, this);

	this->system = system;
}
inline PinScreen &PinScreenHandler::getPinScreen()
{
	return pin_screen;
}
inline void PinScreenHandler::setAnimationBaseDir(std::string const &base_dir)
{
	animation_base_dir = base_dir;
}
inline void PinScreenHandler::rebuildPinScreen()
{
	std::string size = StringToken::lexical_cast<std::string>(h_size.int_value) + "x" + StringToken::lexical_cast<std::string>(v_size.int_value);
	system->print("Rebuilding pin screen with size " + size + "...");
	system->print("Level Of Detail is: " + StringToken::lexical_cast<std::string>(ps_lod.bool_value) );

	pin_screen.clearPinScreen();
	pin_screen.setSize(h_size.int_value, v_size.int_value);
	pin_screen.setLod(ps_lod.int_value);
	pin_screen.setDivisionNumber(ps_division.int_value);
	pin_screen.setLodDistance(ps_lod_level_distance.float_value);
	pin_screen.buildPinScreen();

	if (ps_max_movement.float_value != 0)
	{
		pin_screen.overrideMaxMovement(ps_max_movement.float_value);
	}
	
	system->handleCommand("/set ps_draw_mode absolute");
	system->handleCommand("/echo rebuild completed.");
}
inline void PinScreenHandler::setImagesDir(std::string images_dir)
{
	this->images_dir = images_dir;
}