#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <ctime>

#include "kernel_types.h"

namespace fs {

	struct file_t;
	typedef std::pair<std::string, file_t*> dir_entry_t;

	struct file_t {

		std::string file_name;
		//std::vector<char> data;
		std::string data;
		//size_t length;
		file_type type;
		std::time_t timestamp;
		std::vector<dir_entry_t> children;


		file_t(const char* name, file_type type, time_t timestamp = time(0), file_t* parent = nullptr);
		size_t size() { return children.size() * sizeof(char); }
		~file_t();
	};
}
