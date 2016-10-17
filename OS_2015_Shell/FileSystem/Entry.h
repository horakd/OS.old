#pragma once

#include <string>

namespace file_system
{
	enum FileType
	{
		regular,
		directory
	};

	class Entry
	{
		friend class Directory;
	protected:
		std::string name_;
		FileType type_;
		Entry(const std::string& file_name, FileType type);
		
	public:
		virtual ~Entry();

		inline const std::string& name() const { return name_; }
		inline const FileType& type() const { return type_; }
	};

}

