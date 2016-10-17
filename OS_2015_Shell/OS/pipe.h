#pragma once

#include <mutex>

#include "os_structs.h"

struct Pipe {
	char *buffer;
	int start;
	int end;
	PCB *from;
	PCB *to;
	std::mutex mutex;
};

int create_pipe(PCB *from, PCB *to, Pipe **output);


//bool pipe_can_write(Pipe *pipe);

//0 - ok, 1 - eof, 2 - full
int pipe_write(Pipe *pipe, char *input, size_t size);


//bool pipe_can_read(Pipe *pipe);

//0 - ok, 1 - eof, 2 - empty
int pipe_read(Pipe *pipe, char *output, size_t size);

