#pragma once
#include "os_structs.h"
#include "filesystem.h"
#include "pipe.h"

int k_init();

int k_syscall(OSCall *call);

int k_run();

void k_shutdown(int code);
