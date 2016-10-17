#pragma once

#include <string>

#include "TokenIterator.h"

namespace utils {


	class StringTokenizer
	{
	public:
		const std::string tokens;
		const std::string string;
		const TokenIterator end_;

		StringTokenizer(const std::string& string, const std::string& tokens);

		TokenIterator begin();
		const TokenIterator& end() const { return end_; }
	};

	

}