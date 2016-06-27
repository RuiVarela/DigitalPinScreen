#include "../Headers/System.h"
#include "../Headers/StringToken.h"

#include <sstream>
#include <stdlib.h>

using namespace std;
using namespace StringToken;

void SystemVariable::set(const char *value)
{
	if (! (var_flags & (read_only | latch)) )
		strcpy(string_value,value);
}
void SystemVariable::set(const float value)
{
	if (! (var_flags & (read_only | latch)) )
	{
		sprintf(string_value,"%f",value);
		float_value = value;
	}
}
void SystemVariable::set(const int value)
{
	if (! (var_flags & (read_only | latch)) )
	{
		sprintf(string_value,"%i",value);
		int_value = value;
	}
}
void SystemVariable::set(const bool value)
{
	if (! (var_flags & (read_only | latch)) )
	{	
		sprintf(string_value,"%i",value);
		bool_value = value;
	}
}
void SystemVariable::unlatch()
{
	if (! (var_flags & read_only) )
		var_flags &= ~latch;
}
void SystemVariable::reset()
{
	strcpy(this->string_value,default_string);

	switch(var_type)
	{
	case float_var:
		float_value = atof(default_string);
		break;
	case int_var:
		int_value = atoi(default_string);
		break;
	case bool_var:
		if( (strcmp(default_string,"true") == 0) || (strcmp(default_string,"1") == 0) )
			bool_value = true;
		else if( (strcmp(default_string,"false") == 0) || (strcmp(default_string,"0") == 0) )
			bool_value = false;
		else
			throw GfxException(402,"Invalid data value");
		break;
	default:
		break;
	}
}
void System::registerCommand(char const * name, int const id, ISystemCommand *const system_command)
{
	int len = strlen(name);

	if ((len > 0) && (len < max_name_lenght) ) 
	{
		SystemCommand sys_cmd; 
		sys_cmd.command = system_command;
		sys_cmd.id = id;
		strcpy(sys_cmd.name, name);

		commands[string(name)] = sys_cmd;
	}
	else
		throw GfxException(3001,"Invalid Command Name");
}
void System::registerVariable(SystemVariable *system_variable, ISystemVariable * variable_handler)
{
	int len = strlen(system_variable->getName());

	if ((len > 0) && (len < max_name_lenght) ) 
	{
		system_variable->var_handler = variable_handler;
		variables[string(system_variable->getName())] = system_variable;
	}
	else
		throw GfxException(3002,"Invalid variable Name");
}
void System::print(char *const data) const
{
	if (console != 0)
		console->addMessage(data);
}
void System::print(std::string const &message) const
{
	if (console != 0)
		console->addMessage(message);
}
void System::handleCommand(char *data)
{
	bool invalid = false;

	if (data[0] == '\0')
		print("Empty input");
	else if (data[0] != '/')
		print("Not a command : " + std::string(data));
	else if (data[1] != '\0')
	{
		// extract the command name
		char cmd[max_name_lenght];
		int j = 1;
		while((data[j] != ' ') && (data[j] != '\0'))
		{
			cmd[j - 1] = data[j];
			++j;
		}
		cmd[j - 1] = '\0';

		hash_map<string, SystemCommand>::iterator i = commands.find(cmd);

		if (i != commands.end())
			(*i).second.command->handleSystemCommand( (*i).second.id, &data[j]);
		else
			invalid = true;
	}
	else
		invalid = true;

	if (invalid)
		print("Invalid command : " + std::string(data));

}
void System::handleSystemCommand(int const id, char *parameters)
{
	switch(id)
	{
	case help_command :
		help();
		break;
	case list_command :
		list();
		break;
	case clear_command:
		clear();
		break;
	case quit_command:
		quit();
		break;
	case echo_command:
		echo(parameters);
		break;
	case set_command:
		set(parameters);
		break; 
	case version_command:
		version();
		break;
	default:
		print("Unbound command typed");
		break;
	}
}

bool System::handleSystemVariable(SystemVariable *const system_variable, char *parameters)
{
	bool state_ok = false;

	if (system_variable == &dummy)
	{
		print("Bypassing dummy");
		dummy.set(parameters);
	}

	return state_ok;
}

/*
* Basic commands
*
*/
void System::quit()
{
	exit(0);
}
void System::help()
{
	print("Help :");
	print("No help available.. ask Ryven to start coding");
	print("Try /list.");
}
void System::list()
{
	print("Available commands :");
	for(hash_map<string, SystemCommand>::iterator i = commands.begin(); i != commands.end(); ++i)
		print(string("/") + (*i).second.name);
}
void System::version()
{
	print(system_version.getString());
}
void System::clear()
{
	console->clearMessageBuffer();
}
void System::echo(char *data)
{
	if (data[0] == ' ')
		print(&data[1]);
}
void System::set(char *data)
{
	int len = strlen(data);

	if (len == 0) // List Variables
	{
		print("Defined variables :");
		for(hash_map<string, SystemVariable*>::iterator i = variables.begin(); i != variables.end(); ++i)
		{
			// print the var flags
			ostringstream ostring;
			ostring 
				<< ( ((*i).second->flags() & SystemVariable::read_only) ? 'R' : '.' )
				<< ( ((*i).second->flags() & SystemVariable::latch) ? 'L' : '.' )
				<< ( ((*i).second->flags() & SystemVariable::archived) ? 'A' : '.' )
				<< ' ' << (*i).first
				<< " . " << (*i).second->getString();

			print(ostring.str());
		} 
	}
	else
	{
		char var_name[SystemVariable::max_lenght];
		char *next;
		next = copyToken(var_name,trimLeft(data));

		hash_map<string, SystemVariable*>::iterator i = variables.find(var_name);

		if (i != variables.end()) // the variable exists
		{
			if (*next == '\0') // just print the var
			{
				ostringstream ostring;
				ostring 
				<< ( ((*i).second->flags() & SystemVariable::read_only) ? 'R' : '.' )
				<< ( ((*i).second->flags() & SystemVariable::latch) ? 'L' : '.' )
				<< ( ((*i).second->flags() & SystemVariable::archived) ? 'A' : '.' )
				<< ' ' << (*i).second->getString();

				print(ostring.str());
			} 
			else // set the var
			{
				SystemVariable *var = (*i).second;

				if (var->flags() & SystemVariable::read_only)
					print("The Variable is read only.");
				else if(var->flags() & SystemVariable::latch)
					print("The Variable is latched.");
				else
				{
					bool change = true;

					if ( var->var_handler != 0) //if has a handler installed
					{
						change = var->var_handler->handleSystemVariable( (*i).second , next);
					}
					else if ( change && (var->type() != SystemVariable::string_var) && (numberOfTokens(next) > 1) )
					{		 
						print("Invalid Arguments.");
						print("/set <var name> <var value>");
						change = false;
					}

					if (change)
					{
						switch(var->type())
						{
						case SystemVariable::int_var :
							var->set( atoi(next) );
							break;

						case SystemVariable::float_var :
							var->set( float(atof(next)) );
							break;

						case SystemVariable::string_var :
							var->set(next);
							break;

						case SystemVariable::bool_var :
							var->set( bool(atoi(next)) );
							break;

						default :
							break;
						}

					}

				}
			}
		}	
		else   
			print("Invalid Variable.");
	}
}

