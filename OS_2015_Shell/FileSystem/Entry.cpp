#include "Entry.h"



namespace file_system
{
	Entry::Entry(const std::string& file_name, FileType type) :
		name_(file_name),
		type_(type)
	{

	}


	Entry::~Entry()
	{
	}

}
