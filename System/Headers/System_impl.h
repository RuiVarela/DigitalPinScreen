#include <sstream>
#include <stdio.h>

inline SystemVariable::SystemVariable(char *const name,char * const value,Type const type, int const flags)
:var_flags(flags), var_type(type)
{
	strcpy(this->name,name);
	strcpy(this->default_string,value);

	reset();
}
inline SystemVariable::SystemVariable(char *const name,char * const value, int const flags)
:var_flags(flags), var_type(string_var)
{
	strcpy(this->name,name);

	strcpy(this->string_value,value);
	strcpy(this->default_string,value);
}
inline SystemVariable::SystemVariable(char *const name,int const value, int const flags)
:var_flags(flags), var_type(int_var)
{
	strcpy(this->name,name);

	int_value = value;
	sprintf(this->string_value,"%i",value);
	sprintf(this->default_string,"%i",value);
}
inline SystemVariable::SystemVariable(char *const name,float const value, int const flags)
:var_flags(flags), var_type(float_var)
{
	strcpy(this->name,name);

	float_value = value;
	sprintf(this->string_value,"%f",value);
	sprintf(this->default_string,"%f",value);
}
inline SystemVariable::SystemVariable(char *const name,bool const value, int const flags)
:var_flags(flags), var_type(bool_var)
{
	strcpy(this->name,name);

	bool_value = value;
	sprintf(this->string_value,"%i",value);
	sprintf(this->default_string,"%i",value);
}
inline SystemVariable::SystemVariable()
:var_type(undefined_var)
{
}
inline SystemVariable::~SystemVariable()
{
}
inline const char *const SystemVariable::getName() const
{
	return name;
}
inline int SystemVariable::flags() const
{
	return var_flags;
}
inline SystemVariable::Type SystemVariable::type() const
{
	return var_type;
}
inline const char * SystemVariable::getString() const
{
	return string_value;
}
inline int SystemVariable::getInt() const
{
	return int_value;
}
inline bool SystemVariable::getBool() const
{
	return bool_value;
}
inline float SystemVariable::getFloat() const
{
	return float_value;
}

/**
*
* System
*
**/

inline System::System()
:console(0)
,system_version("version","Digital Pin Screen Prototype One V29.11.06 P3D 2006-2007", SystemVariable::read_only)
,dummy("dummy","This is a dummy var")
{
	registerCommand("help", help_command, this );
	registerCommand("version", version_command, this );
	registerCommand("list", list_command, this );
	registerCommand("clear", clear_command, this );
	registerCommand("echo", echo_command, this );   
	registerCommand("set", set_command, this );
	registerCommand("quit", quit_command, this );

	registerVariable(&system_version);  
	registerVariable(&dummy, this);  
}
inline System::System(Console * const console)
{
	setConsole(console);
}
inline System::~System()
{
}
inline void System::setConsole(Console * const console)
{
	this->console = console;
	console->setCommandHandler(this);
}
inline Console *System::getConsole() const
{
	return console;
}
template<typename Type> void System::print(Type const &message) const
{
	std::ostringstream out;
	out << message;
	print(out.str());
}
