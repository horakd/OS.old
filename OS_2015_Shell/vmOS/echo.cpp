#include "echo.h"


int echo(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv)
{
	for (auto arg : argv)
	{
		clib_prints(output, arg);
	}
	return 0;
}
