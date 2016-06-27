#include "../Headers/Script.h"
#include "../Headers/StringToken.h"

void Script::run()
{
	if (system)
	{
		for (int i = 0; i != commands.size(); ++i)
		{
			char *c_str = new char[ commands[i].size() + 1 ];
			strcpy(c_str,commands[i].c_str());    
			c_str[ commands[i].size() ] = '\0';

			system->handleCommand(c_str);
		}
	}
}
Script::Script(std::istream& in)
:system(0)
{
	int commands_number;
	in >> commands_number >> StringToken::cleanLine;

	for (int i = 0; i != commands_number; ++i)
	{
		std::string command;
		getline(in, command);
		commands.push_back(command);
	}

	if (!in)
		throw GfxException(7002,"File error, script cannot be loaded.");
}

void Script::saveTo(std::ostream& out) const
{
	out << int(commands.size()) << std::endl;

	for (int i = 0; i != commands.size(); ++i)
	{
		out << commands[i] << std::endl;
	}

	if (!out)
		throw GfxException(7001,"File error, script cannot be saved.");

}