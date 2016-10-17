#include "TokenIterator.h"
#include "StringTokenizer.h"

namespace utils
{
	TokenIterator::TokenIterator(const StringTokenizer& const tokenizer, std::size_t start) :
		tokenizer_(tokenizer),
		prev(start)
	{
		pos = tokenizer_.string.find_first_of(tokenizer_.tokens, prev);
		if (pos == std::string::npos) 
			pos = tokenizer_.string.length();
	}


	bool TokenIterator::operator==(const TokenIterator& other) { return prev == other.prev; }

	std::string TokenIterator::operator*() { return tokenizer_.string.substr(prev, pos - prev); }

	void TokenIterator::operator++()
	{
		prev = pos + 1;
		pos = tokenizer_.string.find_first_of(tokenizer_.tokens, prev);
		if (pos == std::string::npos)
		{
			if (prev > tokenizer_.string.length()) prev = pos;
			else pos = tokenizer_.string.length();
		}
	}

}


