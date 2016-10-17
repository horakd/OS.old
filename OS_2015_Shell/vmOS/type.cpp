#include "type.h"
#include "clib.h"

int type(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv)
{
	char line[MAX_LINE_LEN];

	for (auto arg : argv)
	{
		auto file = clib_open_file(arg, FLAG_READ);
		if (file != INVALID_HANDLE)
		{
			if (argv.size() > 1)
			{
				clib_prints(STD_ERR, arg);
				clib_prints(STD_ERR, "\n");
			}
			while (clib_get_line(file, line, MAX_LINE_LEN))
			{
				clib_prints(output, line);
				clib_prints(output, "\n");
			}
			clib_close_file(file);
		}
		else
		{
			clib_prints(STD_ERR, arg);
			clib_prints(STD_ERR, ": cannot open file.\n");
		}
			
		
	}
	return 0;
}
