#include "Utils.h"
namespace file_system
{
	std::vector<std::string> split_path(const std::string & path_string)
	{
		std::vector<std::string> elems;

		std::size_t prev = 0, pos;
		while ((pos = path_string.find_first_of(delimiters, prev)) != std::string::npos)
		{
			if (pos > prev)
				elems.push_back(path_string.substr(prev, pos - prev));
			prev = pos + 1;
		}
		if (prev < path_string.length())
			elems.push_back(path_string.substr(prev, std::string::npos));

		return elems;
	}
	Entry * entryAt(Directory& relative_dir, std::string path)
	{
		auto eles = split_path(path);
		Directory* dir = &relative_dir;

		for (auto it = eles.cbegin(); it != eles.cend(); it++)
		{
			if (it == eles.cbegin() && it->length() == 0)
				continue;

			if ((eles.cend() - it) != 1)
			{
				Entry* f = (*dir)[*it];
				if (f != nullptr && f->type() == directory)
					dir = static_cast<Directory*>(f);
				else
					return nullptr;
			}
			else
			{
				Entry* f = (*dir)[*it];
				return f;
			}
		}

		return nullptr;
	}

	File * fileAt(Directory & relative_dir, std::string path)
	{
		Entry* f = entryAt(relative_dir, path);
		if (f != nullptr && f->type() == regular)
			return static_cast<File*>(f);
		else
			return nullptr;
	}

	std::stringstream* openFile(Directory & relative_dir, std::string path)
	{
		auto f = fileAt(relative_dir, path);
		std::stringstream* ss = nullptr;
		if (f != nullptr)
		{
			ss = &f->open();
		}
		return ss;
	}

	Directory * directoryAt(Directory & relative_dir, std::string path)
	{
		Entry* f = entryAt(relative_dir, path);
		if (f != nullptr && f->type() == directory)
			return static_cast<Directory*>(f);
		else
			return nullptr;
	}

}