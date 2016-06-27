#include "GfxException.h"

#include <fstream>

inline Script::Script()
:system(0)
{
}
inline Script::Script(std::string const &filename)
:system(0)
{
	std::ifstream in(filename.c_str());
	Script copy(in);
	copy.setSystem(system);
	in.close();
	*this = copy;
}
inline void Script::addCommand(std::string const& line)
{
	commands.push_back(line);
}
inline void Script::setSystem(System *system)
{
	this->system = system;
}
inline std::string const& Script::operator[](int const index) const
{
	return commands[index];
}
inline std::string & Script::operator[](int const index)
{
	return commands[index];
}
inline void Script::loadFrom(std::istream& in)
{
	Script copy(in);
	copy.setSystem(system);
	*this = copy;
}
inline void Script::saveToFile(std::string const &filename)
{
	std::ofstream out(filename.c_str());
	saveTo(out);
	out.close();
}