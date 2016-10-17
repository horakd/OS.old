#include <vector>
#include <iostream>

#include "Shell.h"

int main()
{
	fs::FileSystem fs;
	cmd::Shell s(fs);	
	s.run(std::cin);

	return 0;
}