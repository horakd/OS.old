#pragma once

#include "stdafx.h"

#define BUFFER_SIZE 10

typedef struct _pipe {
	int* buffer;
	int write_position;
	int read_position;

	int in_closed;
	int out_closed;

	int is_keyboard;

	CRITICAL_SECTION lockBuffer;
	CONDITION_VARIABLE notEmptyBuffer;
	CONDITION_VARIABLE notFullBuffer;
} Pipe;

typedef struct _pipe_out {
	Pipe* pipe;
	int auto_close;
} Pipe_out;


typedef struct _pipe_in {
	Pipe* pipe;
	int auto_close;
} Pipe_in;

int create_pipe(Pipe_in* in, Pipe_out* out, int aclose_in, int aclose_out);
int create_pipe_kb(Pipe_in* in, Pipe_out* out, int aclose_in, int aclose_out, int is_keyboard);

void close_pipe_in(Pipe_in* in);
void close_pipe_out(Pipe_out* out);

void write_to_pipe(Pipe_in* in, int wr);
void write_str_to_pipe(Pipe_in* in, char* str);

int read_from_pipe(Pipe_out* out);
int try_read_from_pipe(Pipe_out* out);

int keyboard_pipe_out(Pipe_out* out);
