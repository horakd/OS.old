#include "RAMDrive.h"

namespace file_system
{

	RAMDrive::RAMDrive() : root_(nullptr, "root")
	{
	}

	RAMDrive::~RAMDrive()
	{
	}

}