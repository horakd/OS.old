#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace file_system
{

	class Path
	{
	private:
		std::vector<std::string> elements_;
		std::string path_;

	public:
		Path(const std::string& path);
		~Path();

		inline const std::string& path_string() { return path_; }
		inline const std::vector<std::string>& elements() { return elements_; }
	};

}

