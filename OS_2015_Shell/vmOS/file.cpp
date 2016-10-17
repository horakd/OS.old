#include "file.h"
#include <cassert>


namespace fs
{
	file_t::file_t(const char * name, file_type type, time_t timestamp, file_t * parent) :
		file_name(name),
		type(type)
	{
		if (type == file_type::directory)
		{
			children.push_back(dir_entry_t(".", this));
			children.push_back(dir_entry_t("..", parent));
		}
		else if(type == file_type::file) {
			//data.push_back('\0');
		}
		else assert(false);
	}

	file_t::~file_t()
	{
		for (auto f : children)
		{
			if (f.first != "." && f.first != "..")
				delete f.second;
		}
	}
}


