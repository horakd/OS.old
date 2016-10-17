#pragma once

#include <vector>

#include "File.h"
#include "FileIterator.h"

namespace fs {

	class FileSystem
	{
	private:
		static FileIterator leaf_;
		File root_;
		//std::vector<File*> drives_;

	public:
		FileSystem() :
			root_("root")
		{
			root_.add_child(new File("C:"));
		}

		FileIterator root() { return FileIterator(&root_); }
		FileIterator begin() { return FileIterator(root_["C:"]); }
		inline const FileIterator& leaf() const { return leaf_; }
		const FileIterator& end() const { return leaf_; }

		~FileSystem() { }
	};


}



