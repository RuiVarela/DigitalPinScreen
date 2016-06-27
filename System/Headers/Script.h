#ifndef __SYSTEM_SCRIPT__
#define __SYSTEM_SCRIPT__

#include <iosfwd>
#include <vector>
#include <string>

#include "System.h"

class Script {
public:
	Script();
	Script(std::string const &filename);
	Script(std::istream& in);

	void addCommand(std::string const& line);
	std::string const& operator[](int const index) const;
	std::string & operator[](int const index);

	void run();

	void loadFrom(std::istream& in);
	void saveTo(std::ostream& out) const;
	void saveToFile(std::string const &filename);

	void setSystem(System *system);
private:
	System *system;
	std::vector<std::string> commands;
};
//std::istream& operator>>(std::istream& in, Script &script);
//std::ostream& operator<<(std::ostream& out, Script const &script);

#include "Script_impl.h"

#endif //__SYSTEM_SCRIPT__