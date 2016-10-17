#pragma once

#include <vector>
#include <string>

#include "clib.h"
#include "shell_types.h"


int echo(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv);

