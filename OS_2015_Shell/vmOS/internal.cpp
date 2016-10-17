#include "internal.h"
#include <set>
#include <stdlib.h> 
#include <time.h> 
#include <chrono>
#include <thread>
#include <conio.h>
#include <unordered_map>

#define SLOW_DOWN
#define EOF_C 0x1A


os::dir_handle_t mkdir(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv)
{
	for (auto arg : argv)
	{
		clib_open_dir(arg, FLAG_CREATE);
	}
	return 0;
}

os::dir_handle_t chdir(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv)
{
	if (argv.size() > 1)
		return INVALID_HANDLE;
	if (argv.size() == 0)
	{
		char buffer[0xFFF];
		auto dir = clib_open_dir(".", FLAG_READ);
		if (clib_get_path(dir, buffer, 0xFFF) == buffer)
		{
			//std::reverse(buffer, buffer + strlen(buffer));
			clib_prints(buffer);
			clib_close_file(dir);
		}
		else
		{
			clib_close_file(dir);
			return INVALID_HANDLE;
		}
		return 0;
	}
	else if (argv.size() == 1)
	{
		auto ndir = clib_open_dir(argv[0], FLAG_CREATE | FLAG_CHANGE);
		if (ndir != INVALID_HANDLE)
		{
			clib_close_file(ndir);
			return 0;
		}
		else return INVALID_HANDLE;
	}
	else return INVALID_HANDLE;
}

int dir(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv)
{

	char buffer[MAX_LINE_LEN + 1];
	if (argv.size() == 0) argv.push_back("."); //little hack
	for (auto arg : argv)
	{
		auto dir = clib_open_dir(arg, FLAG_READ);
		if (dir != INVALID_HANDLE)
		{
			clib_prints(output, "\n");
			clib_prints(output, "Directory of ");
			if (clib_get_path(dir, buffer, 0xFFF) == buffer)
				clib_prints(output, buffer);
			clib_prints(output, "\n\n");

			while (clib_get_line(dir, buffer, MAX_LINE_LEN))
			{
				clib_prints(output, buffer);
				clib_prints(output, "\n");
			}
			//clib_prints(output, "\n");
			clib_close_file(dir);
		}
	}
	return 0;
}

int sort(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv)
{
	char line[MAX_LINE_LEN + 1];
	std::set<std::string> lines;
	
	if (input != INVALID_HANDLE)
	{
		while (clib_get_line(input, line, MAX_LINE_LEN))
		{
			lines.insert(line);
		}

		for (auto l : lines)
		{
			clib_prints(output, l.c_str());
			clib_prints(output, "\n");
		}
			
	}
	else
	{
		clib_prints(STD_ERR, "Invalid file.\n");
	}
	return 0;
}

int rand(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv)
{
	srand((unsigned int)time(NULL));
	char buffer[0x1f];

	char c = '\0';
	while (c != EOF_C)
	{
		sprintf_s(buffer, "%f\n", (float)(rand() / static_cast<float> (RAND_MAX)) );
		clib_prints(output, buffer);
		if (_kbhit()) c = _getche();//win-specific, there is no support in C/C++
#ifdef SLOW_DOWN
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
	}

	return 0;
}

int freq(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv)
{
	char line[MAX_LINE_LEN + 1];
	size_t freq[0x100];
	memset(freq, 0, sizeof(freq));

	for (auto arg : argv)
	{
		auto file = clib_open_file(arg, FLAG_READ);
		if (file != INVALID_HANDLE)
		{
			while (clib_get_line(file, line, MAX_LINE_LEN))
			{
				for (size_t i = 0; line[i] != '\0'; ++i)
					freq[line[i]]++;
			}
			clib_close_file(file);
		}
		else
		{
			clib_prints(STD_ERR, arg);
			clib_prints(STD_ERR, ": cannot open file.\n");
		}
	}

	for (size_t i = 0; i < 0x100; ++i)
	{
		if (freq[i])
		{
			sprintf_s(line, "0x%hhx : %d\n", i, freq[i]);
			clib_prints(output, line);
		}
	}

	return 0;
}

int wc(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv)
{
	char line[MAX_LINE_LEN];
	std::unordered_map<std::string, size_t> wc;

	for (auto arg : argv)
	{
		auto file = clib_open_file(arg, FLAG_READ);
		if (file != INVALID_HANDLE)
		{
			while (clib_get_line(file, line, MAX_LINE_LEN))
			{
				bool endline = false;
				size_t start = 0;
				for (size_t i = 0; i < MAX_LINE_LEN && !endline; i++)
				{
					if (line[i] == ' ' || line[i] == '\0')
					{
						if (line[i] == '\0') endline = true;
						if (start + 1 == i)//skip spaces
							start++;
						else
						{
							line[i] = '\0';
							wc[&line[start]]++;
							start = i + 1;
						}
					}
					else
					{
						line[i] = line[i] > 'Z' ? line[i] - 0x20 : line[i]; //to uppercase(skip)
						if (line[i] > 'Z' || line[i] < 'A') start = i;
					}
				}
			}
			clib_close_file(file);
		}
		else
		{
			clib_prints(STD_ERR, arg);
			clib_prints(STD_ERR, ": cannot open file.\n");
		}
	}

	for (auto i : wc)
	{
		sprintf_s(line, "%s : \t\t %u\n", i.first.c_str(), i.second);
		clib_prints(line);
	}

	return 0;
}

int rd(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv)
{
	for (size_t i = 0; i < argv.size() - 1; i += 2)
	{
		auto file = clib_open_file(argv[i], FLAG_NONE);
		if (file != INVALID_HANDLE)
		{
			clib_rename_file(file, argv[i + 1]);
			clib_close_file(file);
		}
		else
		{
			clib_prints(STD_ERR, argv[i]);
			clib_prints(STD_ERR, ": cannot open file.\n");
		}


	}
	return 0;
}
