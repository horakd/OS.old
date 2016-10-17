
#include <thread>
#include <mutex>
#include <condition_variable>

#include "pipe_io.h"

namespace pipe_io {

	void* read(os::file_stream_t & stream, os::pipe_t * pipe, char * data, size_t size)
	{
		std::unique_lock<std::mutex> lock(pipe->mutex);
		pipe->cv.wait(lock, [&pipe] { return pipe->can_read || !pipe->write_open; });
		if (!pipe->write_open) return 0;
		std::memcpy(data, pipe->buffer, size > pipe->size ? pipe->size : size);
		pipe->can_read = false;
		pipe->can_write = true;
		lock.unlock();
		pipe->cv.notify_one();
		return (void*)data;
	}

	void* write(os::file_stream_t & stream, os::pipe_t * pipe, char * data, size_t size)
	{
		std::unique_lock<std::mutex> lock(pipe->mutex);
		pipe->cv.wait(lock, [&pipe] {return pipe->can_write || !pipe->read_open; }); 
		if (!pipe->read_open) return 0;
		std::memcpy(pipe->buffer, data, size > pipe->size ? pipe->size : size);
		pipe->buffer[size > pipe->size ? pipe->size : size] = '\0';
		pipe->can_read = true;
		pipe->can_write = false;
		lock.unlock();
		pipe->cv.notify_one();
		return (void*)data;
	}

}


