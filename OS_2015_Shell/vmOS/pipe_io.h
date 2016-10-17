#pragma once

#include "kernel_types.h"

namespace pipe_io {

	void* read(os::file_stream_t & stream, os::pipe_t * pipe, char * data, size_t size);
	void* write(os::file_stream_t & stream, os::pipe_t * pipe, char * data, size_t size);

}