#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <exception>

#include "Entry.h"
#include "Directory.h"

namespace file_system
{
	const char* const delimiters = "\\/:";

	std::vector<std::string> split_path(const std::string &path_string);

	Entry* entryAt(const Directory& relative_dir, std::string path);
	std::stringstream* openFile(Directory& relative_dir, std::string path);
	File* fileAt(Directory& relative_dir, std::string path);
	Directory* directoryAt(Directory& relative_dir, std::string path);
}