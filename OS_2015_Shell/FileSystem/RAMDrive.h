#pragma once
#include "Directory.h"

namespace file_system
{

	class RAMDrive
	{
		friend class RAMDisk;
		
	private:
		char drive_letter_;
		Directory root_;

	public:
		RAMDrive();
		~RAMDrive();

		Directory& root() { return root_; }
	};

}
