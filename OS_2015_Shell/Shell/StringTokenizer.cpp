#include "StringTokenizer.h"

namespace utils {

	StringTokenizer::StringTokenizer(const std::string & string, const std::string & tokens) :
		tokens(tokens),
		string(string),
		end_(*this, std::string::npos)
	{ }

	TokenIterator StringTokenizer::begin() { return TokenIterator(*this, 0); }
}
