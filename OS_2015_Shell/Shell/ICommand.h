#pragma once

#include <string>
#include "FileSystem.h"



namespace cmd {

	static const int RESULT_OK = 0;

	class ICommand
	{
	protected:
		ICommand() { }
		static const std::string UNKNOWN_ERR;


	public:
		virtual const std::string& error_message(int error_code) const = 0;
		virtual int execute(fs::FileIterator& working_dir, const std::string& args, const std::stringstream& input, std::ostream& output) = 0;

		int operator()(fs::FileIterator& working_dir, const std::string& args, const std::stringstream& input, std::ostream& output);

		virtual ~ICommand() { }
	};

}