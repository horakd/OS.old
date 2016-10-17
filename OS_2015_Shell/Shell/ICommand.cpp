#include "ICommand.h"

namespace cmd {

	const std::string ICommand::UNKNOWN_ERR = "Unknown error";

	int ICommand::operator()(fs::FileIterator & working_dir, const std::string& args, const std::stringstream & input, std::ostream & output)
	{
		return execute(working_dir, args, input, output);
	}

}