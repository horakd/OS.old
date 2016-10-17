#include "File.h"


namespace file_system
{

	File::File(Directory* parent, const std::string& file_name) :
		Entry(file_name, regular),
		parent_(parent)
	{

	}


	File::~File()
	{
	}

	std::stringstream& File::open()
	{
		return data_stream_;
	}
	void File::rename(const std::string & file_name)
	{
		if (parent_ != nullptr)
		{
			parent_->contents_.erase(name_);
			parent_->contents_[file_name] = this;
		}
		this->name_ = file_name;
	}
}
