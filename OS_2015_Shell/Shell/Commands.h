#pragma once

#include <sstream>
#include <regex>
#include <algorithm>

#include "FileSystem.h"
#include "StringTokenizer.h"
#include "ICommand.h"

namespace cmd {

	class Make_dir : public ICommand {
	private:
		static const std::string ALREADY_EXIST;

	public:
		// Inherited via ICommand
		virtual int execute(fs::FileIterator & working_dir, const std::string& args, const std::stringstream & input, std::ostream & output) override;
		virtual const std::string & error_message(int error_code) const override;
	};

	class Change_dir : public ICommand {

	private:
		fs::FileSystem& file_system_;
		static const std::string INVALID_PATH;
		static const std::string NOT_FOUND;

	public:
		// Inherited via ICommand
		virtual int execute(fs::FileIterator & working_dir, const std::string& args, const std::stringstream & input, std::ostream & output) override;
		virtual const std::string & error_message(int error_code) const override;

		Change_dir(fs::FileSystem& file_system);
	};


	class Echo_text : public ICommand {

	private:

	public:
		// Inherited via ICommand
		virtual int execute(fs::FileIterator & working_dir, const std::string& args, const std::stringstream & input, std::ostream & output) override;
		virtual const std::string & error_message(int error_code) const override;
	};

	//wc
	class Word_count : public ICommand {

	private:

	public:
		// Inherited via ICommand
		virtual int execute(fs::FileIterator & working_dir, const std::string& args, const std::stringstream & input, std::ostream & output) override;
		virtual const std::string & error_message(int error_code) const override;
	};

	//cat http://stackoverflow.com/questions/7868936/read-file-line-by-line
}

