#include "console_io.h"
#include <iostream>
#include <cstdio>


void* console::read(char* line, size_t limit, char delim = '\n')
{
	std::cin.getline(line, limit, delim);
	if (std::cin.eof())
		return nullptr;
	return line;
}

void* console::read(char * line, size_t limit)
{
	std::cin.getline(line, limit);
	return line;
}

void* console::write(char * buffer, size_t limit)
{
	std::cout.write(buffer, limit);
	return 0;
}
