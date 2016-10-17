#pragma once

#include <unordered_map>
#include <sstream>
#include <string>
#include <iostream>

#include "FileSystem.h"
#include "Commands.h"

namespace cmd {

	class Shell
	{
	private:
		fs::FileIterator working_dir_;
		fs::FileSystem& file_system_;
		bool end_ = false;

		std::unordered_map<std::string, ICommand*> commands_;


	public:
		Shell(fs::FileSystem& file_system) :
			file_system_(file_system),
			working_dir_(file_system.begin())
		{
			commands_["md"] = new Make_dir();
			commands_["cd"] = new Change_dir(file_system_);
			commands_["echo"] = new Echo_text();
			commands_["wc"] = new Word_count();
		}

		std::string trim(std::string& str)
		{
			size_t first = str.find_first_not_of(' ');
			size_t last = str.find_last_not_of(' ');
			return str.substr(first, (last - first + 1));
		}

		void run(std::istream& stream)
		{
			std::string token;
			std::string args;

		cmdloop:
			while (!end_)
			{
				(*commands_["cd"])(working_dir_, "", std::stringstream(""), std::cout);
				std::cout << ">";

				std::getline(stream, args);
				auto scommand = std::stringstream(args);

				std::stringstream input = std::stringstream("");
				std::stringstream output = std::stringstream("");
				std::string argsc = std::string(args);


				while (argsc.size() > 0)
				{
					size_t cmd_lng = argsc.find('|');
					if (cmd_lng == std::string::npos)
						if ((cmd_lng = argsc.find('>')) == std::string::npos)
							cmd_lng = argsc.size();

					std::string one_cmd = trim(argsc.substr(0, cmd_lng));
					int pos = one_cmd.find(' ');
					pos = (pos == std::string::npos) ? one_cmd.size() : pos;
					std::string name = one_cmd.substr(0, pos);
					std::string argv = (pos == one_cmd.size()) ? "" : one_cmd.substr(pos + 1, one_cmd.size());

					auto cmd = commands_.find(name);

					if (cmd != commands_.end())
					{
						int result = cmd->second->execute(working_dir_, argv, input, output);
						input = std::stringstream(output.str());
						output = std::stringstream("");
						if (result != cmd::RESULT_OK)
						{
							std::cerr << cmd->second->error_message(result) << std::endl;
							goto cmdloop;
						}
					}
					else if (name == "exit")
					{
						end_ = true;
						goto cmdloop;
					}
					else
					{
						std::cerr << "Unknown command" << std::endl;
						goto cmdloop;
					}

					//prepare to next step
					//argsc = trim(argsc);
					argsc = argsc.substr(one_cmd.size(), argsc.size());
					if (argsc.size() != 0)
						argsc = trim(argsc);
					int p;
					if (p = argsc.find('>') == 0)
					{
						bool append = (argsc[1] == '>');
						argsc = argsc.substr((append) ? 2 : 1, argsc.size());
						//TODO write_to_file(argsc, true, output);
						std::cout << "file: " << argsc << " append: " << append << " text: " << input.str() << std::endl;
						goto cmdloop;
					}
					else {
						cmd_lng = argsc.find('|');
						if (cmd_lng != std::string::npos)
							argsc = trim(argsc.substr((cmd_lng == argsc.size()) ? cmd_lng : cmd_lng + 1, argsc.size()));

					}
				}
				std::cout << input.str() << std::endl;
			}
		}

		~Shell();
	};
}


