#include "stdafx.h"
#include "pipe.h"

int create_pipe_kb(Pipe_in* in, Pipe_out* out, int close_in, int close_out, int is_keyboard)
{
	Pipe* pipe = (Pipe*)malloc(sizeof(Pipe));

	pipe->buffer = (int*)(malloc(sizeof(int) * BUFFER_SIZE));
	for (int i = 0; i < BUFFER_SIZE; i++)
		pipe->buffer[i] = 0;

	pipe->write_position = 0;
	pipe->read_position = 0;
	pipe->in_closed = 0;
	pipe->out_closed = 0;
	pipe->is_keyboard = is_keyboard;

	InitializeCriticalSection(&(pipe->lockBuffer));
	InitializeConditionVariable(&(pipe->notEmptyBuffer));
	InitializeConditionVariable(&(pipe->notFullBuffer));

	in->pipe = pipe;
	out->pipe = pipe;

	in->auto_close = close_in;
	out->auto_close = close_out;

	return 0;
}

int create_pipe(Pipe_in* in, Pipe_out* out, int close_in, int close_out) 
{
	return create_pipe_kb(in, out, close_in, close_out, 0);
}

void close_pipe_in(Pipe_in* in) 
{
	if (in->auto_close) 
	{
		write_to_pipe(in, -1);

		if (in->pipe->out_closed == 1)
			free(in->pipe);
		else
			in->pipe->in_closed = 1;

		free(in);
	}
}

void close_pipe_out(Pipe_out* out) 
{
	if (out->auto_close) 
	{
		while (read_from_pipe(out) != -1) {};
		
		if (out->pipe->in_closed == 1)
			free(out->pipe);
		else 
			out->pipe->out_closed = 1;

		free(out);
	}
}

void write_to_pipe(Pipe_in* in, int wr) 
{
	//	printf("pipe %d - write\n", pipe_in);
	Pipe* pipe = in->pipe;

	EnterCriticalSection(&(pipe->lockBuffer));
	int new_write_pos = (pipe->write_position + 1) % BUFFER_SIZE;

	while (pipe->read_position == new_write_pos) {
		/* buffer is full, sleeping */
		SleepConditionVariableCS(&(pipe->notEmptyBuffer), &(pipe->lockBuffer), INFINITE);
	}

	pipe->buffer[pipe->write_position] = wr;
	pipe->write_position = new_write_pos;

	LeaveCriticalSection(&(pipe->lockBuffer));
	WakeConditionVariable(&(pipe->notEmptyBuffer));
}

void write_str_to_pipe(Pipe_in* in, char* str) 
{
	int length = strlen(str);

	for (int i = 0; i < length; i++) 
		write_to_pipe(in, str[i]);
}

int try_read_from_pipe(Pipe_out* out) 
{
	//printf("pipe %d - try read\n", pipe_out);
	Pipe* pipe = out->pipe;
	int val;

	EnterCriticalSection(&(pipe->lockBuffer));
	if (pipe->read_position == pipe->write_position)
		val = -2;
	else 
	{
		val = pipe->buffer[pipe->read_position];
		pipe->read_position = (pipe->read_position + 1) % BUFFER_SIZE;
	}

	LeaveCriticalSection(&(pipe->lockBuffer));
	WakeConditionVariable(&(pipe->notFullBuffer));

	return val;
}

int read_from_pipe(Pipe_out* out) 
{
	//	printf("pipe %d - read\n", pipe_out);
	Pipe* pipe = out->pipe;
	int val;

	EnterCriticalSection(&(pipe->lockBuffer));
	while (pipe->read_position == pipe->write_position)
		SleepConditionVariableCS(&(pipe->notEmptyBuffer), &(pipe->lockBuffer), INFINITE);

	val = pipe->buffer[pipe->read_position];
	if (val != -1) pipe->read_position = (pipe->read_position + 1) % BUFFER_SIZE;

	LeaveCriticalSection(&(pipe->lockBuffer));
	WakeConditionVariable(&(pipe->notFullBuffer));

	return val;
}


int keyboard_pipe_out(Pipe_out* out)
{
	return out->pipe->is_keyboard;
}
