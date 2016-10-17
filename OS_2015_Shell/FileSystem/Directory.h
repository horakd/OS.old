#pragma once
#include "Entry.h"
#include "File.h"
#include <algorithm>
#include <map>

namespace file_system
{
	class Directory :
		public Entry
	{
		friend class FileFactory;
		friend class File;
	private:
		static constexpr char* const invalid_chars = "\\/:";
		static constexpr char* const parent_name = "..";

		std::map<std::string, Entry* > contents_;

	public:
		Directory(Directory* const parent_dir, const std::string& file_name);
		~Directory();

		
		inline const std::map<std::string, Entry* >& contents() const { return contents_; }
		const Directory* create_directory(const std::string& file_name);
		const File* create_file(const std::string& file_name);
		void rename(const std::string& file_name);
		void print_contents(std::ostream& stream);



		Entry* operator[](const std::string& file_name) const {
			auto ptr = contents_.find(file_name);
			return ptr == contents_.end() ? nullptr : ptr->second;
		}
	};

}