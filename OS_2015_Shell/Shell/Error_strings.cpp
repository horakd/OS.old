#pragma once


#include "Commands.h"

namespace cmd {

	const std::string Change_dir::INVALID_PATH = "Invalid path";
	const std::string Change_dir::NOT_FOUND = "Directory not found";

	const std::string Make_dir::ALREADY_EXIST = "One or more directories already exist";

}
