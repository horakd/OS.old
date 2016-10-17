#include "kernel.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <queue>

#include <vector>

int(*sysproc[128])(void *params);

std::vector<PCB*> pcbTab;
std::vector<FileH*> fileHTab;
std::vector<Pipe*> pipeTab;
std::queue<OSCall*> osCallQ;

std::mutex osCallQm;
std::mutex kernel_work_mutex;
std::condition_variable kernel_work_cond;

DirEntry *root;

bool kernel_run = true;


int _createProcess(void *params)
{
	CreateProcess *cp = (CreateProcess*)params;

	return 0;
}

int _createPipe(void *params)
{
	return 0;
}

int _write(void *params)
{
	RWContainer *c = (RWContainer*)params;
	FileH *handler = c->handler;
	FILE *f;
	Pipe *p;
	char input = c->character;
	if (handler->rights != WRITE)
		return 1; //TODO error have not rights
	switch (handler->type)
	{
		case FT_FILE:
			f = (FILE*)handler->file;
			fputc(input, f);
			break;
		case FT_PIPE:
			p = (Pipe*)handler->file;
			return pipe_write(p, (char*)input, sizeof(input));
			break;
		case FT_STREAM:
			std::string *s = (std::string*)handler->file;
			s += input;
			break;
	}

	return 0;
}

void _initFS()
{
	root = (DirEntry*)malloc(sizeof(DirEntry));

	if (fs_createFile(root, true) != 0)
	{
		std::cerr << "Fatal error creating filesystem";
		exit(1);
	}
	fs_setRoot(root, true);
	root->parent = nullptr;
	root->name = "root";
}

void _createFolderStructure()
{
	DirEntry *b = (DirEntry*)malloc(sizeof(DirEntry));
	fs_createFile(b, true);
	b->name = "B";
	fs_addDirEntry(b, root);

	DirEntry *b1 = (DirEntry*)malloc(sizeof(DirEntry));
	fs_createFile(b1, true);
	b1->name = "b1";
	fs_addDirEntry(b1, b);

	DirEntry *b2 = (DirEntry*)malloc(sizeof(DirEntry));
	fs_createFile(b2, true);
	b2->name = "b0";
	fs_addDirEntry(b2, b);

	DirEntry *c = (DirEntry*)malloc(sizeof(DirEntry));
	fs_createFile(c, true);
	c->name = "C";
	fs_addDirEntry(c, root);

	DirEntry *a = (DirEntry*)malloc(sizeof(DirEntry));
	fs_createFile(a, true);
	a->name = "A";
	fs_addDirEntry(a, root);

}

void _initSysProcesses()
{
	sysproc[0] = &_createProcess;
	sysproc[1] = &_createPipe;
	sysproc[2] = &_write;
}

//int read(FileH *handler, std::string output, size_t size)
//{
//	int result = -1;
//	switch (handler->type)
//	{
//	case FILE:
//
//		break;
//	case PIPE:
//
//		break;
//	case STREAM:
//
//		break;
//	}
//
//	return result;
//}

int k_init()
{
	pcbTab = std::vector<PCB*>();
	fileHTab = std::vector<FileH*>();
	pipeTab = std::vector<Pipe*>();
	osCallQ = std::queue<OSCall*>();

	_initFS();
	_createFolderStructure();
	_initSysProcesses();

	fs_printTree(root, 0);

	return 0;
}

PCB* findPCB()
{
	size_t i;
	for (i = 0; i < pcbTab.size(); i++)
		if (pcbTab.at(i)->thread.get_id() == std::this_thread::get_id())
			return pcbTab.at(i);
	return nullptr;
}

int k_syscall(OSCall *call)
{
	call->pcb = findPCB();

	osCallQm.lock();
	osCallQ.push(call);
	osCallQm.unlock();
	//wakeup kernel
	kernel_work_cond.notify_all();

	//uspání akutálního vlákna

	std::unique_lock<std::mutex> lck(call->pcb->mtx);
	call->pcb->cv.wait(lck);

	return 0;
}

bool osCallQnonEmpty()
{
	bool res;
	osCallQm.lock();
	res = (osCallQ.size() > 0);
	osCallQm.unlock();
	return res;
}

int k_run()
{

	//nekoneèný cyklus
	while (kernel_run)
	{
		std::unique_lock<std::mutex> locker(kernel_work_mutex);
		kernel_work_cond.wait(locker, osCallQnonEmpty);

		osCallQm.lock();
		OSCall *c = osCallQ.front();
		osCallQ.pop();
		osCallQm.unlock();

		//vykonání fce
		int ret = sysproc[c->service](c->params);

		//probuzení vlákna
		c->pcb->cv.notify_all();

	}

	//uvolnìní pamìti
	//výpisy...

	return 0;

}

void k_shutdown(int code)
{
	kernel_run = false;
	//probuzení kernelu

}
