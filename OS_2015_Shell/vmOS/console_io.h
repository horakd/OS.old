#pragma once

#include "kernel_types.h"

namespace console {

	void * read(char * line, size_t limit, char delim);
	void * read(char * line, size_t limit);
	void * write(char * buffer, size_t limit);
}

