#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>

#include "kernel.h"
#include "shell.h"
#include "console_io.h"
#include "pipe_io.h"
#include "file_system_io.h"


#define SYS_VEC_LEN 0xFF
#define TOTAL_FILE_LIM 0xFFF
#define PROC_FILE_LIM 0x3F
#define SYS_DEVICES_LEN 0x3

#define CNSL_DEVICEI 0x0
#define PIPE_DEVICEI 0x1

namespace os {

	typedef void*(*syscall_t)(int argc, void ** argv);

	struct process_context {
		fs::file_t* working_dir;
		file_stream_t open_files[PROC_FILE_LIM];
		std::thread* this_thread;
	};

	struct file_ref {
		file_type type;
		size_t read_ref_count = 0;
		size_t write_ref_count = 0;
		void* file = nullptr;
	};

	
	struct {
		syscall_t sysc_vector[SYS_VEC_LEN];
		file_ref file_table[TOTAL_FILE_LIM];

		std::unordered_map<std::thread::id, process_context*> PCB;
		fs::file_t* fs_root;

		bool shut_down_sig;
		std::thread::id kernel_thread_id;
	} OS_data;


	void main_loop();
	void clean_up();

	file_handle_t get_file(void* file)
	{
		file_handle_t handle = INVALID_HANDLE;
		for (file_handle_t i = SYS_DEVICES_LEN; i < TOTAL_FILE_LIM; ++i)
		{
			if (handle == INVALID_HANDLE && OS_data.file_table[i].file == nullptr) handle = i; //first empty
			if (OS_data.file_table[i].file == file) return i; //if already open
		}
		return handle;
	}

	file_handle_t get_loc_stream(file_stream_t* filev)
	{
		for (int i = SYS_DEVICES_LEN; i < PROC_FILE_LIM; ++i)
		{
			if (filev[i].g_handle == -1)
			{
				filev[i].r_pos = filev[i].w_pos = 0;
				filev[i].eof = false;
				return i;
			}
		}
		return INVALID_HANDLE;
	}

	process_context* get_thread_context()
	{
		return OS_data.PCB[std::this_thread::get_id()];
	}


	void run()
	{
		for (auto i = 0; i < SYS_VEC_LEN; ++i) 
			OS_data.sysc_vector[i] = &os::null_call;

		OS_data.sysc_vector[READ] = &sys_read;
		OS_data.sysc_vector[SYS_WRITE_S] = &sys_write;
		OS_data.sysc_vector[CREATE_PIPE] = &sys_create_pipe;
		OS_data.sysc_vector[RUN] = &sys_run;
		OS_data.sysc_vector[OPEN_FILE] = &sys_open_file;
		OS_data.sysc_vector[GET_PATH] = &sys_get_path;
		OS_data.sysc_vector[CLOSE_FILE] = &sys_close_file;
		OS_data.sysc_vector[RENAME_FILE] = &sys_rename_file;
		
		OS_data.file_table[STD_IN].type = file_type::cnsl_io;
		OS_data.file_table[STD_OUT].type = file_type::cnsl_io;
		OS_data.file_table[STD_ERR].type = file_type::cnsl_io;
		OS_data.file_table[STD_IN].read_ref_count++;
		OS_data.file_table[STD_OUT].write_ref_count++;
		OS_data.file_table[STD_ERR].write_ref_count++;

		OS_data.fs_root = new fs::file_t("root", file_type::directory);
		auto c = new fs::file_t("C:", file_type::directory);
		c->children.pop_back(); //break parent-child relation
		OS_data.fs_root->children.push_back(fs::dir_entry_t("C:", c));

		auto pc = new process_context();
		pc->open_files[STD_IN].mode = FLAG_READ;
		pc->open_files[STD_IN].g_handle = STD_IN;
		pc->open_files[STD_OUT].mode = FLAG_WRITE;
		pc->open_files[STD_OUT].g_handle = STD_OUT;
		pc->open_files[STD_ERR].mode = FLAG_WRITE;
		pc->open_files[STD_ERR].g_handle = STD_ERR;
		pc->working_dir = c;
		OS_data.kernel_thread_id = std::this_thread::get_id();
		OS_data.PCB[OS_data.kernel_thread_id] = pc;
		

		void* params[6];
		params[0] = (void*)&shell;
		params[1] = (void*)STD_IN;
		params[2] = (void*)STD_OUT;
		params[3] = (void*)STD_ERR;
		params[4] = (void*)0;
		params[5] = (void*)nullptr;

		auto t = (std::thread*)sys_run(6, params);
		t->join();
		delete t;

		clean_up();
	}
	
	void clean_up()
	{
		delete OS_data.PCB[OS_data.kernel_thread_id];
		delete OS_data.fs_root;
	}

	void* syscall(int service, int argc, void ** argv)
	{
		return OS_data.sysc_vector[service](argc, argv);
	}

	void* null_call(int argc, void ** argv) {
		return nullptr;
	}

	void* sys_open_file(int argc, void ** argv)
	{
		assert(argc == 4);
		char* name = (char*)argv[0];
		open_mode flags = (int)argv[1];
		file_type* type = (file_type*)argv[2];
		file_handle_t handle = (file_handle_t)argv[3]; //handle to override
		auto ctx = OS_data.PCB[std::this_thread::get_id()];

		if ((*type == file_type::file) || (*type == file_type::directory/* && flags & FLAG_CREATE*/))
		{
			auto f = fs::open(ctx->working_dir, *type, name, flags);
			if (f == nullptr ||  f->type != *type) return (void*)INVALID_HANDLE;


			file_handle_t global_handle = get_file(f);
			assert(global_handle != INVALID_HANDLE);

			//change a std stream
			if ((*type == file_type::file) && (handle == STD_IN || handle == STD_OUT || handle == STD_ERR))
			{
				//close old one here
				ctx->open_files[handle].eof = false;
				ctx->open_files[handle].r_pos = ctx->open_files[handle].w_pos = 0;
				ctx->open_files[handle].g_handle = global_handle;
				return (void*)handle;
			}

			file_handle_t local_handle = get_loc_stream(ctx->open_files);
			assert(local_handle != INVALID_HANDLE);
			auto& f_ref = OS_data.file_table[global_handle];
			auto& s_ref = ctx->open_files[local_handle];
			if (f_ref.file == f)//already open
			{
				if ((!(flags & FLAG_WRITE)) && f_ref.write_ref_count == 0) //read-only
				{
					if(flags & FLAG_READ) f_ref.read_ref_count++;
				}
				else return (void*)INVALID_HANDLE;
			}
			else if (f_ref.file == nullptr) // not yet open
			{
				if (flags & FLAG_READ) f_ref.read_ref_count++;
				if (flags & FLAG_WRITE) f_ref.write_ref_count++;
				f_ref.file = f;
			}
			else assert(false);

			s_ref.g_handle = global_handle;
			s_ref.mode = flags;

			if ((flags & FLAG_CHANGE) && f->type == file_type::directory) ctx->working_dir = f;

			return (void*)local_handle;
		}
		return (void*)INVALID_HANDLE;
	}

	void * sys_rename_file(int argc, void ** argv)
	{
		assert(argc == 2);
		char* name = (char*)argv[0];
		file_handle_t handle = (file_handle_t)argv[1];
		auto ctx = OS_data.PCB[std::this_thread::get_id()];
		auto entry = OS_data.file_table[ctx->open_files[handle].g_handle];
		auto f = (fs::file_t*)OS_data.file_table[ctx->open_files[handle].g_handle].file;
		if (f == nullptr || (entry.type != file_type::file && entry.type != file_type::directory))
		{
			return (void*)INVALID_HANDLE;
		}
		if (fs::is_name_valid(f, name))
			f->file_name = name;

		return (void*)handle;
	}

	void * sys_close_file(int argc, void ** argv)
	{
		assert(argc == 1);
		file_handle_t handle = (file_handle_t)argv[0];
		if ((handle == INVALID_HANDLE) || (handle >= PROC_FILE_LIM)) return (void*)INVALID_HANDLE; //argument out of range

		auto ctx = OS_data.PCB[std::this_thread::get_id()];
		auto& stream = ctx->open_files[handle];
		auto& f_ref = OS_data.file_table[stream.g_handle];

		if (stream.mode & FLAG_READ) f_ref.read_ref_count--;
		if (stream.mode & FLAG_WRITE) f_ref.write_ref_count--;

		if (f_ref.read_ref_count == 0 && f_ref.type == file_type::pipe) //notify other end of pipe
			((pipe_t*)f_ref.file)->read_open = false;

		if (f_ref.write_ref_count == 0 && f_ref.type == file_type::pipe) //notify other end of pipe
			((pipe_t*)f_ref.file)->write_open = false;

		if ((f_ref.read_ref_count == 0) && (f_ref.write_ref_count == 0))
		{
			if (f_ref.type == file_type::pipe) delete ((pipe_t*)f_ref.file);
			f_ref.file = nullptr; //recycle
		}

		//reset stream data
		stream.eof = false;
		stream.g_handle = INVALID_HANDLE;  //recycle
		stream.mode = FLAG_NONE;
		stream.r_pos = stream.w_pos = 0;

		return (void*)handle;
	}

	void * sys_get_path(int argc, void ** argv)
	{
		assert(argc == 3);
		file_handle_t fd = (file_handle_t)argv[0];
		assert(fd <= PROC_FILE_LIM);
		char* data = (char*)argv[1];
		size_t limit = (size_t)argv[2];

		auto& fd_vec = OS_data.PCB[std::this_thread::get_id()]->open_files;
		auto& f_ptr = OS_data.file_table[fd_vec[fd].g_handle];

		if ((f_ptr.type == file_type::file) || (f_ptr.type == file_type::directory))
		{
			return fs::get_path( (fs::file_t*)f_ptr.file, limit, data);
		}
		return nullptr;
	}

	void* sys_read(int argc, void ** argv) {
		assert(argc == 3 || argc == 4);
		file_handle_t fd = (file_handle_t)argv[0];
		assert(fd < PROC_FILE_LIM);
		char* data = (char*)argv[1];
		size_t limit = (size_t)argv[2];
		char* delim = (char*)argv[3];
		assert(delim != nullptr);

		auto& fd_vec = OS_data.PCB[std::this_thread::get_id()]->open_files;
		auto& f_ref = OS_data.file_table[fd_vec[fd].g_handle];
		assert(fd_vec[fd].mode & FLAG_READ);

		if (f_ref.type == file_type::cnsl_io)
		{
			return console::read(data, limit, *delim);
		}
		else if (f_ref.type == file_type::pipe)
		{
			return pipe_io::read(fd_vec[fd], (pipe_t*)f_ref.file, data, limit);
		}
		else if (f_ref.type == file_type::file || f_ref.type == file_type::directory)
		{
			return fs::read(fd_vec[fd], (fs::file_t*)f_ref.file, limit, data, *delim);
		}

		return nullptr;
	}

	void* sys_write(int argc, void ** argv) {
		assert(argc == 3);
		os::file_handle_t fh = (file_handle_t)argv[0];
		//os::file_stream_t* f_ptr = OS_data.file_table[OS_data.PCB[std::this_thread::get_id()]->open_files[fh]];
		auto& stream = OS_data.PCB[std::this_thread::get_id()]->open_files[fh];
		auto& f_ref = OS_data.file_table[stream.g_handle];
		assert(stream.mode & FLAG_WRITE);

		char* data = (char*)argv[1];
		size_t size = (size_t)argv[2];

		if (f_ref.type == file_type::cnsl_io)
		{
			console::write(data, size);
		}
		else if (f_ref.type == file_type::pipe)
		{
			pipe_io::write(stream, (pipe_t*)f_ref.file, data, size);
		}
		else if (f_ref.type == file_type::file)
		{
			fs::write(stream,(fs::file_t*)f_ref.file, 0, size, data);
		}
		else assert(false);
	  
		return nullptr;
	}
	
	void * sys_create_pipe(int argc, void ** argv)
	{
		assert(argc == 3);
		auto ctx = OS_data.PCB[std::this_thread::get_id()];
		size_t size = (size_t)argv[2];
		auto p = new pipe_t(size);

		file_handle_t global_handle = get_file(p);

		file_handle_t r_handle = get_loc_stream(ctx->open_files);
		if(r_handle == INVALID_HANDLE || global_handle == INVALID_HANDLE)
		{
			delete p;
			return (void*)false;
		}

		auto& sr_ref = ctx->open_files[r_handle];
		sr_ref.g_handle = global_handle;
		sr_ref.mode = FLAG_READ;

		file_handle_t w_handle = get_loc_stream(ctx->open_files);
		if (w_handle == INVALID_HANDLE)
		{
			sr_ref.g_handle = INVALID_HANDLE;
			delete p;
			return (void*)false;
		}

		auto& sw_ref = ctx->open_files[w_handle];
		sw_ref.g_handle = global_handle;
		sw_ref.mode = FLAG_WRITE;

		auto& f_ref = OS_data.file_table[global_handle];
		f_ref.read_ref_count++;
		f_ref.write_ref_count++;
		f_ref.file = p;
		f_ref.type = file_type::pipe;

		*(file_handle_t*)argv[0] = r_handle;
		*(file_handle_t*)argv[1] = w_handle;

		return (void*)true;
	}

	void proccess_entry_point(process_context* pc, process_entry_point entry, int argc, char ** argv)
	{
		OS_data.PCB[std::this_thread::get_id()] = pc;
		entry(argc, argv);
		void* param[1];
		for (int i = SYS_DEVICES_LEN; i < PROC_FILE_LIM; ++i) //close all files
		{
			os::file_handle_t file = pc->open_files[i].g_handle;
			param[0] = (void*)file;
			sys_close_file(1, param);
		}
		OS_data.PCB.erase(std::this_thread::get_id());
		delete pc;
	}

	void * sys_run(int argc, void ** argv)
	{
		assert(argc == 6);
		process_entry_point main = (process_entry_point)argv[0];
		file_handle_t in = (file_handle_t)argv[1];
		file_handle_t out = (file_handle_t)argv[2];
		file_handle_t err = (file_handle_t)argv[3];
		int a = (int)argv[4];
		char ** v = (char **)argv[5];
		auto& ctx = *OS_data.PCB[std::this_thread::get_id()];
		auto pc = new process_context();

		auto fd_vec = ctx.open_files;

		//pc->working_dir = ctx.working_dir;
		for (auto i = 0; i < PROC_FILE_LIM; ++i)
			pc->open_files[i].g_handle = INVALID_HANDLE;
		pc->open_files[STD_IN] = fd_vec[in];
		pc->open_files[STD_OUT] = fd_vec[out];
		pc->open_files[STD_ERR] = fd_vec[err];
		OS_data.file_table[fd_vec[in].g_handle].read_ref_count++;
		OS_data.file_table[fd_vec[out].g_handle].write_ref_count++;
		OS_data.file_table[fd_vec[err].g_handle].write_ref_count++;
		pc->working_dir = ctx.working_dir;
		pc->this_thread = new std::thread(proccess_entry_point, pc, main, a, v);
		//pc->this_thread.detach();
		return (void*)(pc->this_thread);
	}

}


