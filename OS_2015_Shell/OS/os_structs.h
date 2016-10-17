#pragma once

#include <stdlib.h>
#include <thread>
#include <mutex>

/*
PCB
FileHandler
CreateProc
OSCall
...
*/

 enum FileT {
	FT_FILE, FT_PIPE, FT_STREAM
} ;

enum FileR {
	READ, WRITE
} ;

struct FileH {
	FileT type;
	FileR rights;
	void* file;
};

enum PStatus {
	WAIT, RUN
} ;

struct PCB{
	std::mutex mtx;
	std::condition_variable cv;
	std::thread thread;
	PStatus status;
	std::string base_path;
	std::string curr_path;
	FileH *input;
	FileH *output;
	FileH *error;
	struct _pcb *parent;
};

struct OSCall
{
	int service;
	void* params;
	PCB* pcb;
};

struct CreateProcess
{
	OSCall *call;
	std::string base_path, curr_path;
	FileH *input, *output, *error;
};

struct RWContainer
{
	char character;
	FileH *handler;
};

