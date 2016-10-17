#include "Shell.h"

namespace cmd {

	Shell::~Shell()
	{
		for (auto c : commands_)
			delete c.second;
	}

	

}
