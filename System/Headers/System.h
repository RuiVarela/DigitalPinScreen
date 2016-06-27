/**
* OpenGl Workbench
* Ryven 2006
*
* System
*
* SystemVariables
* SystemCommands
*
**/

#ifndef __SYSTEM__
#define __SYSTEM__

#include <vector>
#include <string>
#include <hash_map>

#include "GfxException.h"
#include "Console.h"

struct ISystemVariable;

class SystemVariable 
{
public: 
	static int const max_lenght = 256;

	enum Flag
	{ 
		none = 0,
		archived = 1,
		read_only = 2,
		latch = 4
	};

	enum Type
	{
		undefined_var,
		int_var,
		float_var,
		string_var,
		bool_var
	};

	SystemVariable(char *const name,char * const value,Type const type, int const flags);
	SystemVariable(char *const name,char * const value, int const flags = 0);
	SystemVariable(char *const name,int const value, int const flags = 0);
	SystemVariable(char *const name,float const value, int const flags = 0);
	SystemVariable(char *const name,bool const value, int const flags = 0);
	SystemVariable();
	~SystemVariable();

	void set(const char *value);
	void set(const float value);
	void set(const int value);
	void set(const bool value);

	const char *const getName() const;
	int flags() const;
	Type type() const;

	const char *getString() const;
	int getInt() const;
	bool getBool() const;
	float getFloat() const;

	void unlatch();
	void reset();    

	union
	{	
		float float_value;
		int int_value;
		bool bool_value;
	};
private :
	char name[max_lenght];
	char string_value[max_lenght];
	char default_string[max_lenght];

	int var_flags;
	Type var_type;

	ISystemVariable *var_handler;

	friend class System;
};

struct ISystemVariable
{
	virtual ~ISystemVariable() {};
	virtual bool handleSystemVariable(SystemVariable *const system_variable, char *parameters) = 0;
};

struct ISystemCommand
{
	virtual ~ISystemCommand() {};
	virtual void handleSystemCommand(int const id, char *parameters) = 0;
};

class System : ICommandHandler , ISystemCommand , ISystemVariable
{
public:
	static int const max_name_lenght = 65; 

	struct SystemCommand {
		char name[ System::max_name_lenght ];
		int id;
		ISystemCommand *command;
	};

	enum BuiltInCommands
	{
		help_command,
		list_command,
		version_command,
		clear_command,
		echo_command,
		set_command,
		quit_command
	};

	System();
	System(Console * const console);

	virtual ~System();
	void registerCommand(char const * name, int const id, ISystemCommand *const system_command);
	void registerVariable(SystemVariable *system_variable, ISystemVariable * variable_handler = 0);

	void print(char * const data) const;
	void print(std::string const &message) const;
	template<typename Type> void print(Type const &message) const;

	void setConsole(Console * const console);
	Console *getConsole() const;

	// handle console data
	void handleCommand(char *data);

	// handle built int System Commands and varibles
	void handleSystemCommand(int const id, char *parameters);
	bool handleSystemVariable(SystemVariable *const system_variable, char *parameters);

	void help();
	void list();
	void version();
	void clear();
	void echo(char *data);
	void set(char *data);
	void quit();
private:
	std::hash_map<std::string, SystemCommand> commands;
	std::hash_map<std::string, SystemVariable*> variables;
	Console *console;

	SystemVariable system_version;
	SystemVariable dummy;
};


#include "System_impl.h"

#endif // __SYSTEM__