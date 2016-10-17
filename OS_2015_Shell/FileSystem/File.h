#pragma once
#include <sstream>

#include "Entry.h"
#include "Directory.h"

namespace file_system
{

	class File :
		public Entry
	{
	private:
		Directory* parent_;
		std::stringstream data_stream_;

	public:
		File(Directory* parent, const std::string& file_name);
		~File();

		std::stringstream& open();
		void rename(const std::string& file_name);
	};

}