#include "Directory.h"

namespace file_system
{

	Directory::Directory(Directory* const parent_dir, const std::string& file_name) :
		Entry(file_name, directory)
	{
		contents_[parent_name] = parent_dir;
	}


	Directory::~Directory()
	{
		for (auto it = contents_.begin(); it != contents_.end(); ++it)
		{
			if (it->first != parent_name)
			{
				Entry* ptr = it->second;
				delete ptr;
				it->second = nullptr;
			}
		}
	}

	const Directory* Directory::create_directory(const std::string & file_name)
	{
		Directory* dir = nullptr;
		if (file_name.find_first_of(invalid_chars) == std::string::npos && file_name != parent_name)
		{
			auto& entry = contents_[file_name];
			if (entry == nullptr)
			{
				dir = new Directory(this, file_name);
				entry = dir;
			}
		}
		return dir;
	}

	const File* Directory::create_file(const std::string & file_name)
	{
		File* file = nullptr;
		if (file_name.find_first_of(invalid_chars) == std::string::npos && file_name != parent_name)
		{
			auto& entry = contents_[file_name];
			if (entry == nullptr)
			{
				file = new File(this, file_name);
				entry = file;
			}
		}
		return file;
	}

	void Directory::rename(const std::string & file_name)
	{
		auto parent_ptr = contents_.find(parent_name);
		if (parent_ptr != contents_.end())
		{
			Directory* parent = static_cast<Directory*>(parent_ptr->second);
			parent->contents_.erase(name_);
			parent->contents_[file_name] = this;
		}
		this->name_ = file_name;
	}
	void Directory::print_contents(std::ostream& stream)
	{
		for (const auto& item : contents_)
			stream << item.first << std::endl;
	}
}
