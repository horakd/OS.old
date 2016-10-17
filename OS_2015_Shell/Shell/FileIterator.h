#pragma once

#include "File.h"
#include <string>

namespace fs {

	class FileIterator
	{
		friend class FileSystem;
	private:
		File* ptr_; //only single indirection to keep iterator valid when removing children
		FileIterator() : ptr_(nullptr) {}

	public:
		FileIterator& operator=(const FileIterator& it) { 
			ptr_ = it.ptr_; 
			return *this;
		}

		FileIterator& moveDown(const std::string& file_name) { ptr_ = ptr_->child(file_name); return *this; }
		FileIterator& moveUp() { ptr_ = ptr_->parent(); return *this; }

		bool operator==(const FileIterator& it) { return it.ptr_ == ptr_; }
		bool operator!=(const FileIterator& it) { return !(*this == it); }

		File* operator*() const { return ptr_; }

		FileIterator(File* file) : ptr_(file) {}
		FileIterator(const FileIterator& it) : ptr_(it.ptr_) { }
		~FileIterator() { }
	};

}

