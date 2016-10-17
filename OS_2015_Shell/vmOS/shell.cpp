#include "shell.h"

#include "clib.h"
#include <thread>
#include <chrono>
#include <unordered_map>
#include <string>
#include <vector>
#include <cassert>

#include "shell_types.h"
#include "echo.h"
#include "type.h"
#include "internal.h"

//#define TEST_PIPE
//#define TEST_FILE

os::file_handle_t* fs_producer;
os::file_handle_t* fs_consumer;

int shell_cmd(os::file_handle_t output, os::file_handle_t input, std::vector<char*> argv)
{
	auto process = clib_run(&shell, argv.size(), argv.size() ? &argv[0] : nullptr, STD_IN, STD_OUT, STD_ERR);
	process->wait_for_finished();
	delete process;
	return 0;
}


struct 
{
	std::unordered_map<std::string, shell_func> shell_funcs;
	bool is_init = false;	
} data;

void init()
{
	if (!data.is_init)
	{
		data.shell_funcs["cmd"] = &shell_cmd;
		data.shell_funcs["echo"] = &echo; 
		data.shell_funcs["type"] = &type;
		data.shell_funcs["md"] = &mkdir;
		data.shell_funcs["cd"] = &chdir;
		data.shell_funcs["dir"] = &dir;
		data.shell_funcs["sort"] = &sort;
		data.shell_funcs["rand"] = &rand;
		data.shell_funcs["freq"] = &freq;
		data.shell_funcs["wc"] = &wc;
		//data.shell_funcs["rd"] = &rd;
	}
}

int shell(int argc, char ** argv)
{
	
	char buffer[0x80];
	sprintf_s(buffer, "%d", std::this_thread::get_id());
	clib_prints(STD_ERR, "Shell ");
	clib_prints(STD_ERR, buffer);
	clib_prints(STD_ERR, " started...\n");
	
	init();
	bool exit = false;

	bool c_switch = false; 
	bool has_input = false;

	if (argc > 0)
	{
		has_input = true; 
		if (strcmp(argv[0], "/C") == 0)
			c_switch = true;
	}
	

	while (!exit)
	{
		std::vector<char*> cmd_argv; 
		char line[MAX_LINE_LEN];

		if (!has_input)
		{
			chdir(STD_ERR, STD_IN, cmd_argv);
			clib_prints(">");
			clib_get_line(line, MAX_LINE_LEN);
			if (strcmp(line, "exit") == 0) break;
		}
		else
		{
			memcpy(&line[0],  argv[argc - 1], MAX_LINE_LEN);
			has_input = false;
		}
		
		size_t token_s = 0;
		size_t token_e;
		shell_func cmd = nullptr;

		int16_t in_flags = 0;
		int16_t out_flags = 0;
		os::file_handle_t output = STD_OUT;
		os::file_handle_t input = STD_IN;

		bool f_cmd = true;
		bool quoted = false;
		bool f_err = false;
		bool f_arg = false;
		bool i_rdr = false;
		bool o_rdr = false;
		bool f_ok = false;
		bool p_rdr = false;
		bool iml_quote = false;
		bool execute = false;

		auto len = strlen(line);

		for (token_e = token_s; token_e <= len && !p_rdr; ++token_e)
		{
			if (line[token_e] == ' ' || line[token_e] == '\n' || line[token_e] == '\0')
			{
				if (line[token_e] == '\0') execute = true;
				if (!quoted)
				{
					if (line[token_e] == '\n') execute = true;
					if (f_cmd)
					{
						line[token_e] = '\0';
						auto cmd_it = data.shell_funcs.find(&line[token_s]);
						if(cmd_it == data.shell_funcs.end())//unknown cmd
						{

							clib_prints(STD_ERR, "Unknown command");
							f_err = true;
							break; 
						}
						cmd = cmd_it->second;
						f_cmd = false;
						if (strcmp(&line[token_s], "echo") == 0) iml_quote = true;
						if (strcmp(&line[token_s], "cmd") == 0) iml_quote = true;
						token_s = token_e + 1;
					}
					else if (o_rdr || i_rdr)
					{
						line[token_e] = '\0';
						if(o_rdr)
							output = clib_open_file(&line[token_s], out_flags);
						else
							input = clib_open_file(&line[token_s], in_flags);
						execute = true;
					}
					else
					{
						char c_tmp = line[token_e];
						line[token_e] = '\0'; //make a c string
						char* token = &line[token_s];
						if (strcmp(token, ">") == 0)
						{
							out_flags = FLAG_CREATE | FLAG_WRITE;
							o_rdr = true;
							line[token_e - 2] = '\0';
						}
						else if (strcmp(token, ">>") == 0)
						{
							out_flags = FLAG_CREATE | FLAG_WRITE | FLAG_APPEND;
							o_rdr = true;
							line[token_e - 3] = '\0';
						}
						else if (strcmp(token, "<") == 0)
						{
							in_flags = FLAG_READ;
							i_rdr = true;
							line[token_e - 2] = '\0';
						}
						else if (strcmp(token, "|") == 0)
						{
							p_rdr = true;
							line[token_e - 2] = '\0';
						}
						else
						{
							if(token[0] == '"' || line[token_e - 1] == '"') quoted = !quoted;
							if(iml_quote) line[token_e] = c_tmp; //fallback to original value
							if(!iml_quote || cmd_argv.size() == 0) cmd_argv.push_back(token);
						}
						token_s = token_e + 1;
						
					}
				}
			}
		}

		if (!f_err && cmd != nullptr)
		{
			process_t* process;
			char** argv = nullptr;
			if (p_rdr)
			{
				os::file_handle_t fs_producer;
				os::file_handle_t fs_consumer;
				clib_create_pipe(&fs_consumer, &fs_producer, MAX_LINE_LEN);

				argv = new char*[2];
				argv[0] = new char[3];
				argv[0] = "/C";
				argv[1] = new char[sizeof(line)];
				memcpy(argv[1], &line[token_s], sizeof(line) - token_s);
				output = fs_producer;
				process = clib_run(&shell, 2, argv, fs_consumer, STD_OUT, STD_ERR);
				clib_close_file(fs_consumer);
			}
			cmd(output, input, cmd_argv);
			//there was a redirection
			if (output != STD_OUT)  clib_close_file(output);
			if (input != STD_IN) clib_close_file(input);

			if (p_rdr)
			{
				process->wait_for_finished();
				if (argv)
				{
					delete[] argv;
					argv = nullptr;
				}
				delete process;
			}
		}

		clib_prints(STD_OUT, "\n");

		if (c_switch) exit = true;
	}

	//clib_prints(STD_ERR, "Shuting down OS...\n");
	//os::shut_down();

	return 0;
}
