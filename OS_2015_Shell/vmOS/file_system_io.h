#pragma once

#include "file.h"
#include "kernel_types.h"

namespace fs {
	bool is_name_valid(file_t * file, const std::string & str);
	void* read(os::file_stream_t& stream, file_t* file, size_t len, char* buffer, char delim );
	int write(os::file_stream_t& stream, file_t* file, size_t offset, size_t len, char* data);
	file_t* open(file_t * file, file_type type, char * path, os::open_mode mode);
	char* get_path(file_t* file, size_t len, char* buffer);

}
