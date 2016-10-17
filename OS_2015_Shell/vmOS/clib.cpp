#include "clib.h"
#include <vector>
#include <cassert>
#include <thread>

struct p_handle {
	std::thread* thread;
};

void process_t::wait_for_finished()
{
	if (handle->thread != nullptr && handle->thread->joinable())
	{
		handle->thread->join();
		handle->thread = nullptr;
	}
}

process_t::~process_t()
{
	if (handle != nullptr) delete handle->thread;
	delete handle;
}

void clib_prints(const char * str, size_t limit)
{
	clib_prints(STD_OUT, str, limit);
}

void clib_prints(os::file_handle_t file, const char * str, size_t limit)
{
	void* params[3];
	params[0] = (void*)file;
	params[1] = (void*)str;
	params[2] = (void*)(limit == 0 ? strlen(str) : limit);

	os::syscall(SYS_WRITE_S, 3, params);
}

void clib_create_pipe(os::file_handle_t* read, os::file_handle_t* write, size_t size)
{
	void* params[3];
	params[0] = (void*)read;
	params[1] = (void*)write;
	params[2] = (void*)size;


	os::syscall(CREATE_PIPE, 3, params);
}

char* clib_gets(char* buffer, int max_c)
{
	return clib_gets(STD_IN, buffer, max_c);
}

char* clib_get_path(os::file_handle_t file, char * buffer, int max_c)
{
	void* params[3];
	params[0] = (void*)file;
	params[1] = (void*)buffer;
	params[2] = (void*)max_c;

	return (char*)os::syscall(GET_PATH, 3, params);
}

char* clib_gets(os::file_handle_t file, char * buffer, int max_c )
{
	void* params[3];
	params[0] = (void*)file;
	params[1] = (void*)buffer;
	params[2] = (void*)max_c;

	return (char*)os::syscall(READ, 3, params);
}

char* clib_get_line(char* line, int max_c, char delim)
{
	return clib_get_line(STD_IN, line, max_c, delim);
}

char* clib_get_line(os::file_handle_t file, char* line, int max_c, char delim)
{
	void* params[4];
	params[0] = (void*)file;
	params[1] = (void*)line;
	params[2] = (void*)max_c;
	params[3] = (void*)&delim;

	return (char*)os::syscall(READ, 4, params);
}

process_t* clib_run(os::process_entry_point main, int argc, char ** argv)
{
	void* params[6];
	params[0] = (void*)main;
	params[1] = (void*)STD_IN;
	params[2] = (void*)STD_OUT;
	params[3] = (void*)STD_ERR;
	params[4] = (void*)argc;
	params[5] = (void*)argv;

	auto p = new process_t{ new p_handle{ (std::thread*)os::syscall(RUN, 6, params) } };
	
	return p;
}

process_t* clib_run(os::process_entry_point main, int argc, char ** argv, os::file_handle_t in, os::file_handle_t out, os::file_handle_t err)
{
	void* params[6];
	params[0] = (void*)main;
	params[1] = (void*)in;
	params[2] = (void*)out;
	params[3] = (void*)err;
	params[4] = (void*)argc;
	params[5] = (void*)argv;

	auto p = new process_t{ new p_handle{ (std::thread*)os::syscall(RUN, 6, params) } };

	return p;
}

os::file_handle_t clib_create_file(char* path)
{
	return clib_open_file(path, FLAG_CREATE);
}

os::file_handle_t clib_open_file(char * path, int mode)
{
	return clib_freopen(path, mode, INVALID_HANDLE);
}

os::dir_handle_t clib_open_dir(char * path, int mode)
{
	void* params[4];
	file_type f(file_type::directory);
	params[0] = (void*)path;
	params[1] = (void*)mode;
	params[2] = (void*)&f;
	params[3] = (void*)INVALID_HANDLE;

	return (os::file_handle_t)os::syscall(OPEN_FILE, 4, params);
}

os::file_handle_t clib_close_file(os::file_handle_t file)
{
	void* params[1];
	params[0] = (void*)file;
	return (os::file_handle_t)os::syscall(CLOSE_FILE, 1, params);
}

os::file_handle_t clib_freopen(char * path, int mode, os::file_handle_t file)
{
	void* params[4];
	file_type f(file_type::file);
	params[0] = (void*)path;
	params[1] = (void*)mode;
	params[2] = (void*)&f;
	params[3] = (void*)file;

	return (os::file_handle_t)os::syscall(OPEN_FILE, 4, params);
}

os::file_handle_t clib_rename_file(os::file_handle_t file, char * new_name)
{
	void* params[2];
	params[0] = (void*)new_name;
	params[1] = (void*)file;

	return (os::file_handle_t)os::syscall(RENAME_FILE, 2, params);
}

