#include "File.h"
#include <algorithm>

namespace fs {

	File::File(const std::string& name) :
		parent_(nullptr), 
		name_(name)
	{
	}

	File::File(const std::string & name, File * parent) :
		parent_(parent),
		name_(name)
	{
	}


	bool File::rename(const std::string& name)
	{
		if (parent_ != nullptr)
		{
			if (parent_->child(name) == nullptr) 
				name_ = name;
			else return false;
		}
		else name_ = name;

		return true;
	}

	bool File::move(File* new_parent)
	{
		if (new_parent != nullptr && new_parent->add_child(this) != nullptr)
		{
			if (parent_ != nullptr) parent_->remove_child(this);
			parent_ = new_parent;
			return true;
		}
		else return false;
	}

	File* File::add_child(File* const file)
	{
		if (file != nullptr && child(file->name()) == nullptr)
		{
			children_.push_back(file);
			return file;
		}
		else return nullptr;
	}

	File * File::add_child(const std::string & name)
	{
		File* f = nullptr;
		if (child(name) == nullptr)
		{
			f = new File(name, this);
			children_.push_back(f);
		}
		return f;
	}

	File* File::remove_child(File * const file)
	{
		if (children_.end() != children_.erase(std::remove(children_.begin(), children_.end(), file), children_.end()))
			return file;
		else return nullptr;
	}

	File::~File()
	{
		for (auto f : children_) delete f;
	}

	inline void File::print_dir(std::ostream & stream) {
		for (auto f : children_)
			stream << f->name();
	}

	File * File::child(const std::string& name) const
	{
		auto res = std::find_if(children_.begin(), children_.end(), [&](const File* f) { return f->name() == name; });
		return res == children_.end() ? nullptr : *res;
	}
}