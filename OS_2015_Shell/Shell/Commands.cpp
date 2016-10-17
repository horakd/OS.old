#include "Commands.h"

namespace cmd {

	int Change_dir::execute(fs::FileIterator & working_dir, const std::string& args, const std::stringstream & input, std::ostream & output)
	{
		auto str = input.str();

		if (str.length() == 0) //print path
		{
			std::vector<std::string> file_names;
			for (auto it = working_dir; it != file_system_.leaf(); it.moveUp()) //iterate to root
				file_names.push_back((*it)->name()); //need to construct new string? wtf?

			for (auto it = file_names.rbegin(); it != file_names.rend(); ++it) //print reverse
			{
				output << *it << "\\"; //to-do dont print last backslash
			}

		}
		else
		{
			fs::FileIterator working_dir_fallback = working_dir; //if not exist fallback to this
			bool dir_bwd = true;

			if (str[0] == '\\') //change drive to root
			{
				working_dir = file_system_.begin();
				if (str.length() == 1) //the only part of path, return
					return 0;
			}
			else if (str.length() > 1 && str[1] == ':') //change filesystem to root
				working_dir = file_system_.root();

			std::string file_name;
			utils::StringTokenizer tokenizer(str, "\\/");
			for (auto it = tokenizer.begin(); it != tokenizer.end(); ++it)
			{
				file_name = *it;

				if (file_name.length() > 0)
				{
					if (file_name == "..")
					{
						if (dir_bwd) working_dir.moveUp();
						else return 1;
					}
					else
					{
						working_dir.moveDown(file_name);
						dir_bwd = false;
					}
				}
				//ingore doubled separators?

				if (working_dir == file_system_.leaf())
				{
					working_dir = working_dir_fallback;//nor found, use fallback value, throw if needed
					return 2;
				}
			}
		}

		return 0;
	}

	const std::string& Change_dir::error_message(int error_code) const
	{
		switch (error_code)
		{
		case 1:
			return INVALID_PATH;
		case 2:
			return NOT_FOUND;
		default:
			return UNKNOWN_ERR;
			break;
		}
	}

	Change_dir::Change_dir(fs::FileSystem & file_system) : file_system_(file_system)
	{ }

	//doc http://ss64.com/nt/md.html
	int Make_dir::execute(fs::FileIterator & working_dir,const std::string& args, const std::stringstream & input, std::ostream & output)
	{
		auto reg = std::regex("[^\\s\"']+|\"([^\"]*)\"|'([^']*)'");
		std::string sinput = input.str();
		int result = 0;

		for (auto it = std::sregex_token_iterator(sinput.begin(), sinput.end(), reg); it != std::sregex_token_iterator(); it++)
		{
			std::string name = it->str();
			if ((*working_dir)->add_child(name) == nullptr)
				result = 1;
		}

		return result;
	}

	const std::string & Make_dir::error_message(int error_code) const
	{
		switch (error_code)
		{
		case 1:
			return ALREADY_EXIST;
		default:
			return UNKNOWN_ERR;
		}
	}

	int Echo_text::execute(fs::FileIterator & working_dir, const std::string & args, const std::stringstream & input, std::ostream & output)
	{
		output << args;
		return 0;
	}

	const std::string & Echo_text::error_message(int error_code) const
	{
		switch (error_code)
		{
		default:
			return UNKNOWN_ERR;
		}
	}


	int Word_count::execute(fs::FileIterator & working_dir, const std::string & args, const std::stringstream & input, std::ostream & output)
	{

		std::string text;		
		text = input.str();

		//\n
		int n = std::count(text.begin(),text.end(),'\n');
		//words
		int w = std::count(text.begin(), text.end(), ' ');
		if (text != "")
			w++;
		//chars
		int c = text.size();

		output << n << "\t" << w << "\t" << c ;

		return 0;
	}

	const std::string & Word_count::error_message(int error_code) const
	{
		switch (error_code)
		{
		default:
			return UNKNOWN_ERR;
		}
	}

}