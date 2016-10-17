#pragma once

#include <vector>
#include <string>

#include "clib.h"
#include "shell_types.h"


os::dir_handle_t mkdir(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv);
os::dir_handle_t chdir(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv);
int dir(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv);
int sort(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv);
int rand(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv);
int freq(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv);
int wc(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv);
int rd(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv);