#pragma once
#include <cstdint>
#include <thread>
#include <mutex>
#include <condition_variable>

#define READ 2
#define SYS_WRITE_S 3
#define OPEN 4
#define MOUNT 5
#define CREATE_PIPE 6
#define RUN 7
#define OPEN_FILE 8
#define GET_PATH 9
#define CLOSE_FILE 10
#define RENAME_FILE 11

#define STD_IN 0
#define STD_OUT 1
#define STD_ERR 2

#define INVALID_HANDLE -1

#define FLAG_NONE 0x0
#define FLAG_READ 0x2
#define FLAG_WRITE 0x4
#define FLAG_CREATE 0x8
#define FLAG_APPEND 0x10
#define FLAG_CHANGE 0x10

#define MAX_LINE_LEN 0x1000

enum file_type { file, directory, pipe, cnsl_io };

namespace os {


	typedef int(*process_entry_point)(int argc, char** argv);
	typedef int(*read_func)(void* file, size_t offset, size_t len, char* buffer);
	typedef int(*write_func)(void* file, size_t offset, size_t len, char* data);
	typedef void* (*open_func)(void * file, char * path);
	typedef void* (*create_func)(void * file, char * name);

	typedef int32_t file_handle_t;
	typedef int32_t dir_handle_t;
	typedef int32_t open_mode;
	

	struct file_stream_t {
		open_mode mode;
		size_t r_pos = 0;
		size_t w_pos = 0;
		bool eof = false;
		file_handle_t g_handle = INVALID_HANDLE;
	};

	

	
	struct pipe_t {
		size_t size;
		char* buffer;
		std::mutex mutex;
		std::condition_variable cv;

		bool can_read;
		bool can_write;
		bool read_open;
		bool write_open;

		pipe_t(size_t buff_size) : 
			size(buff_size), 
			buffer(new char[buff_size]) ,
			can_read(false),
			can_write(true),
			read_open(true),
			write_open(true)
		{ }

		~pipe_t() { delete buffer; }
	};


}
