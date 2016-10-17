#pragma once

#include "kernel_types.h"

namespace os {
	
	void run();
	void* syscall(int service, int argc, void** argv);
	void shut_down();

	

	void* null_call(int argc, void ** argv);

	void* sys_read(int argc, void ** argv);

	void* sys_open_file(int argc, void ** argv);

	void* sys_rename_file(int argc, void ** argv);

	void* sys_close_file(int argc, void ** argv);

	void* sys_get_path(int argc, void ** argv);

	void* sys_write(int argc, void ** argv);
	
	void* sys_create_pipe(int argc, void ** argv);

	void* sys_run(int argc, void ** argv);


}