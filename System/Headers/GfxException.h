/**
 * OpenGl Workbench
 * Ryven 2006
 *
 * Gfx Exception
 **/
#ifndef __GFXEXCEPTION__
#define __GFXEXCEPTION__

#include <string>

class GfxException
{
public:
	GfxException()
		:error_code_(0),exception_string_("")
	{
	}
	GfxException(int error_code,std::string exception)
		:error_code_(error_code),exception_string_(exception)
	{
	}
	GfxException(std::string exception)
		:error_code_(0),exception_string_(exception)
	{
	}
	virtual ~GfxException()
	{
	}
	virtual int errorCode()
	{
		return error_code_;
	}
	virtual std::string exceptionString()
	{
		return exception_string_;
	}
private:
	int error_code_;
	std::string exception_string_;
};
#endif //__GFXEXCEPTION__
