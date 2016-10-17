#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace fs {

	class File
	{
		friend class FileSystem;
	private:
		std::string name_;
		std::stringstream data_;
		std::vector<File*> children_;
		File* parent_;

		File(const std::string& name);
		File(const std::string& name, File* parent);

	public:
		~File();


		void print_dir(std::ostream& stream);

		const std::string& name() const { return name_; }
		bool rename(const std::string& name);
		bool move(File* new_parent);

		File* parent() const { return parent_; }
		File* child(const std::string& name) const;
		File* operator[](const std::string& name) const { return child(name); }

		File* add_child(File* const file);
		File* add_child(const std::string& name);
		File* remove_child(File* const file);
		File* remove_child(const std::string& name) { return remove_child(child(name)); }
	};

}