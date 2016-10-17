#pragma once


#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>

typedef struct _dirEntry {
	char *name;
	struct _dirEntry *parent;
	size_t attributes;
	time_t created;
	time_t changed;
	time_t touched;
	void *data;
	size_t size;
}DirEntry;


int fs_createFile(DirEntry *d, bool dir);


bool fs_isRoot(size_t attr);

bool fs_isDir(size_t attr);

bool fs_isHidden(size_t attr);

bool fs_isReadOnly(size_t attr);


void fs_setDir(DirEntry *dir,bool set);

void fs_setRoot(DirEntry *dir, bool set);

void fs_setHidden(DirEntry *dir, bool set);

void fs_setReadOnly(DirEntry *dir, bool set);

int fs_addDirEntry(DirEntry *dir, DirEntry *parent);


void fs_printTree(DirEntry *dir, int level);