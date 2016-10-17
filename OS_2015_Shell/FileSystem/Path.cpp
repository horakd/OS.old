#include "Path.h"
#include "Utils.h"

namespace file_system
{
	Path::Path(const std::string & path) :
		elements_(split_path(path)),
		path_(path)
	{
	}

	Path::~Path()
	{
	}
}