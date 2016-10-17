#pragma once

#include <vector>
#include <string>

#include "kernel_types.h"

typedef int(*shell_func)(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv);