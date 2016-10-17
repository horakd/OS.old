#include "pipe.h"

#define PIPE_BUFFER_SIZE 256

int create_pipe(PCB *from, PCB *to, Pipe **output)
{
	Pipe *p = (Pipe*)malloc(sizeof(Pipe));
	if (p == nullptr)
		return 1;
	p->from = from;
	p->to = to;
	p->buffer = (char*)malloc(sizeof(char) * PIPE_BUFFER_SIZE);
	if (p->buffer == nullptr)
		return 1;
	p->start = 0;
	p->end = 0;

	FileH *pipeF = (FileH*)malloc(sizeof(FileH));
	if (pipeF == nullptr)
		return 1;
	pipeF->file = p;
	pipeF->type = FT_PIPE;
	pipeF->rights = WRITE;

	from->output = pipeF;

	FileH *pipeT = (FileH*)malloc(sizeof(FileH));
	if (pipeT == nullptr)
		return 1;
	pipeT->file = p;
	pipeT->type = FT_PIPE;
	pipeT->rights = READ;

	to->input = pipeT;

	output = &p;

	return 0;
}


bool pipe_can_write(Pipe *pipe)
{
	pipe->mutex.lock();
	bool b = (pipe->start != ((pipe->end + 1) % PIPE_BUFFER_SIZE));
	pipe->mutex.unlock();
	return b;
}

int pipe_write(Pipe *pipe, char *input, size_t size)
{
	size_t i;
	for (i = 0; i < size; i++)
	{
		if (!pipe_can_write(pipe))
			return 2;
		pipe->mutex.lock();
		pipe->buffer[pipe->end] = *input;
		pipe->mutex.unlock();
		if ((char)input == EOF)
		{
			//close fileh
			return 1;
		}
		pipe->mutex.lock();
		pipe->end = (pipe->end++) % PIPE_BUFFER_SIZE;
		pipe->mutex.unlock();
		input++;
	}
	return 0;
}


bool pipe_can_read(Pipe *pipe)
{
	pipe->mutex.lock();
	bool b = (pipe->start != pipe->end);
	pipe->mutex.unlock();
	return b;
}

int pipe_read(Pipe *pipe, char *output, size_t size)
{
	size_t i;
	for (i = 0; i < size; i++)
	{
		if (!pipe_can_read(pipe))
			return 2;
		pipe->mutex.lock();
		*output = pipe->buffer[pipe->start];
		pipe->mutex.unlock();
		if ((char)output == EOF)
		{
			//close pipe & fileh
			return 1;
		}
		output++;
		pipe->mutex.lock();
		pipe->start = (pipe->start++) % PIPE_BUFFER_SIZE;
		pipe->mutex.unlock();
	}
	return 0;
}
