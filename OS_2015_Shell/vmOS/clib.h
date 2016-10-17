#pragma once


#include "kernel_types.h"
#include "kernel.h"
#include <string>

struct p_handle;
struct process_t {
	void wait_for_finished();
	p_handle* handle;
	~process_t();
};

void clib_prints(const char * str, size_t limit = 0);

void clib_prints(os::file_handle_t file, const char * str, size_t limit = 0);

void clib_create_pipe(os::file_handle_t* read, os::file_handle_t* write, size_t size);

char * clib_gets(char * buffer, int max_c);

char * clib_get_path(os::file_handle_t file, char* buffer, int max_c);

char * clib_gets(os::file_handle_t file, char * buffer, int max_c);

char * clib_get_line(char * line, int max_c, char delim = '\n');

char * clib_get_line(os::file_handle_t file, char * line, int max_c, char delim = '\n');

process_t* clib_run(os::process_entry_point main, int argc, char ** argv);

process_t* clib_run(os::process_entry_point main, int argc, char ** argv, os::file_handle_t in, os::file_handle_t out, os::file_handle_t err);

os::file_handle_t clib_create_file(char* fn);

os::file_handle_t clib_open_file(char* path, int mode);

os::dir_handle_t clib_open_dir(char * path, int mode);

os::file_handle_t clib_close_file(os::file_handle_t file);

os::file_handle_t clib_freopen(char* path, int mode, os::file_handle_t file);

os::file_handle_t clib_rename_file(os::file_handle_t file, char* new_name);