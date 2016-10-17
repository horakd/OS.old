#include "filesystem.h"
#include <iostream>

int fs_createFile(DirEntry *d, bool dir)
{
	time_t now = time(NULL);
	d->name = (char*)malloc(sizeof(char) * 256);
	d->data = nullptr;
	if (dir)
	{
		d->attributes = 0x80;
	}
	else
	{
		d->attributes = 0;
	}
	d->changed = now;
	d->created = now;
	d->touched = now;
	d->size = 0;

	return 0;
}


bool fs_isDir(size_t attr)
{
	return (attr && 0x80);
}

bool fs_isRoot(size_t attr)
{
	return (attr && 0x40);
}

bool fs_isHidden(size_t attr)
{
	return (attr && 0x20);
}

bool fs_isReadOnly(size_t attr)
{
	return (attr && 0x10);
}


void fs_setDir(DirEntry *dir, bool set)
{
	if (set)
		dir->attributes |= 0x80;
	else
		dir->attributes &= 0x7F;
}

void fs_setRoot(DirEntry *dir, bool set)
{
	if (set)
		dir->attributes |= 0x40;
	else
		dir->attributes &= 0xBF;
}

void fs_setHidden(DirEntry *dir, bool set)
{
	if (set)
		dir->attributes |= 0x20;
	else
		dir->attributes &= 0xDF;

}

void fs_setReadOnly(DirEntry *dir, bool set)
{
	if (set)
		dir->attributes |= 0x10;
	else
		dir->attributes &= 0xEF;
}


bool _sortEntries(DirEntry **a, DirEntry **b)
{
	return (strcmp((*a)->name, (*b)->name) > 0);
}

int fs_addDirEntry(DirEntry *dir, DirEntry *parent)
{
	parent->size++;
	parent->data = (void*)realloc(parent->data, parent->size * sizeof(DirEntry*));
	DirEntry** data = ((DirEntry**)parent->data);

	size_t i = -1;
	if (parent->size > 1)
		for (i = parent->size - 2; i != (size_t)-1; i--)
			if (strcmp(data[i]->name, dir->name) >= 0)
				data[i + 1] = data[i];
			else
				break;
	data[i + 1] = dir;

	dir->parent = parent;

	return 0;
}

///jen pro úèely testování... zatím
void fs_printTree(DirEntry *dir, int level)
{
	size_t i;
	for (i = 0; i < (size_t)level; i++)
		std::cout << "\t";

	std::cout << dir->name << std::endl;
	i = 0;
	for (DirEntry** dir_arr = ((DirEntry**)dir->data); i < dir->size; i++)
	{
		fs_printTree((dir_arr)[i], level + 1);
	}
}
